
#include <string>
#include <iostream>
#include <vector>
#include <stack>
#include <map>
#include <stdexcept>

#include "symbols.h"

using namespace mj;
using namespace std;

Symbol::Symbol(const string name):
    _name(name)
{
}

ostream& Symbol::print(ostream &os) const {
    return os << ":" << name();
}

namespace mj {

    ostream& operator<<(ostream &os, const Symbol& s) {
        return s.print(os);
    }

    ostream& operator<<(ostream &os, const Symbols& s) {
        return os << *s.currentScope();
    }

    ostream& operator<<(ostream& os, const Scope& s) {
        unsigned int depth = s.depth();
        os << " {\n";
        SymbolTable::const_iterator i = s.symbolTable.begin();
        for (; i != s.symbolTable.end(); i++) {
            for (unsigned int d = 0; d <= depth; d++) os << "\t";
            os << *(i->second) << endl;
        }
        for (unsigned int i = 0; i < depth; i++) os << "\t";
        os << "}\n";
        return os;
    }
}
//
// Basic types
//

Type::Type(const string name):
    Symbol(name)
{
}

bool Type::operator==(const Type &t) const {
    return name() == t.name();
}

bool Type::operator!=(const Type &t) const {
    return !(*this == t);
}

bool Type::compatible(const Type &t) const {
    return *this == t;
}


ostream& Type::print(ostream &os) const {
    return os << name();
}



//
// Reference types
//

ReferenceType::ReferenceType(const string name):
    Type(name)
{}

bool ReferenceType::compatible(const Type &t) const {
    return t == NULL_TYPE || *this == t;
}

//
// Array types
//
ArrayType::ArrayType(const Type &valueType):
    ReferenceType("[" + valueType.name() + "]"),
    _valueType(valueType) {

}

Scope::Scope(Scope *parent):
    _parent(parent)
{
}

Scope::~Scope() {
    SymbolTable::iterator it = symbolTable.begin();
    for(;it != symbolTable.end(); it++) {
        delete it->second;
    }
}

void Scope::define(const Symbol &s) {
    const string name = s.name();
    if (symbolTable[name] != NULL) {
        throw runtime_error("Symbol '" + name + "' already defined!");
    }
    symbolTable[name] = &s;
}

const Symbol* Scope::resolve(const string name) {
    SymbolTable::const_iterator it = symbolTable.find(name);
    if( it != symbolTable.end() ) {
        return it->second;
    }

    if (_parent != NULL) {
        return _parent->resolve(name);
    }

    return NULL;
}

unsigned int Scope::depth() const {
    unsigned int d = 0;
    Scope *cur = _parent;
    while (cur != NULL) {
        cur = cur->_parent;
        d++;
    }
    return d;
}

Variable::Variable(const string name, const Type& type):
    Symbol(name),
    _type(type)
{
}

ostream& Variable::print(ostream &os) const {
    return os << name() << " : " << type();
}

ostream& ScopeContainer::print(ostream &os) const {
    printSignature(os);
    return os << this->scope();
}

MethodArguments::MethodArguments(Scope *parentScope): 
    Scope(parentScope), 
    arguments() 
{
}

bool MethodArguments::matchArguments(ArgumentTypes argumentTypes) {
    ArgumentTypes::size_type size = arguments.size();
    if (size != argumentTypes.size()) {
        return false;
    }
    for (unsigned int i = 0; i < size; i++) {
        if (! arguments[i]->compatible(*argumentTypes[i])) {
            return false;
        }
    }
    return true;
}

string MethodArguments::typeSignature() {
    string sig = "(";
    string sep = "";
    for (ArgumentTypes::iterator it = arguments.begin(); 
            it != arguments.end(); ++it) {
        sig += sep;
        sig += (*it)->name();
        sep = ",";
    }
    sig += ")";
    return sig;
}

void MethodArguments::define(const Symbol &s) {
    const Variable *v = dynamic_cast<const Variable*>(&s);
    if (v == NULL) {
        cerr << "ERROR! Expecting variable, got" << s;
        throw runtime_error("Illegal symbol");
    }
    Scope::define(*v);
    arguments.push_back(&v->type());
}

Method::Method(const string name, 
        const MethodType &type):
    ScopeContainer(),
    Variable(name, type),
    methodScope(new MethodScope(&type.arguments()))
{
}

ostream& Method::printSignature(ostream &os) const {
    return Variable::print(os);
}

ostream& Method::print(ostream &os) const {
    return ScopeContainer::print(os);
}

MethodScope::MethodScope (Scope *parent):
    Scope(parent)
{
}

MethodType::MethodType(MethodArguments &arguments, const Type &returnType):
    Type(arguments.typeSignature() + "->" + returnType.name()),
    _type(returnType),
    _arguments(arguments)
{
}

Class::Class(string name, Scope *parentScope):
    ReferenceType(name),
    ScopeContainer(),
    classScope(*(new ClassScope(parentScope, this)))
{
}

ostream& Class::printSignature(ostream &os) const {
    return Type::print(os);
}

ostream& Class::print(ostream &os) const {
    return Type::print(os);
}

ClassScope::ClassScope(Scope *parent, Type *c):
    Scope(parent),
    _c(c)
{
}

const Symbol *ClassScope::resolve(const string name) {
    if (_c->name() == name) {
        return _c;
    }
    return Scope::resolve(name);
}

const Symbol *ClassScope::resolveField(const string name) {
    // don't look in parent scopes
    return symbolTable[name];
}

Program::Program(string name, Scope *parentScope): 
    ScopeContainer(),
    Symbol(name),
    _scope(*(new Scope(parentScope)))
{
}

ostream& Program::printSignature(ostream &os) const {
    return Symbol::print(os);
}

ostream& Program::print(ostream &os) const {
    return ScopeContainer::print(os);
}

Symbols::Symbols():
    scopes(stack<Scope*>())
{
    scopes.push(&global);
}

void Symbols::define(const Symbol &s) {
    currentScope()->define(s);
}

Class& Symbols::enterClassScope(const string name) {
    Class *c = new Class(name, currentScope());
    define(*c);
    enterScope(c->scope());
    return *c;
}

MethodArguments& Symbols::enterMethodArgumentsScope() {
    MethodArguments *ma = new MethodArguments(currentScope());
    enterScope(*ma);
    return *ma;
}

Method& Symbols::enterMethodScope(const string name, const Type &returnType, MethodArguments &arguments) {
    const MethodType *mtype = new MethodType(arguments, returnType);
    const Symbol *existingType = currentScope()->resolve(mtype->name());
    if (!existingType) {
        global.defineMethodAutoType(*mtype);
#ifdef DEBUG
        cout << "new method type: " << *mtype << endl;
    } else {
        cout << "existing method type: " << *existingType << endl;
#endif
    }
    Method* m = new Method(name, *mtype);
    define(*m);
    enterScope(m->scope());
    return *m;
}

Program& Symbols::enterProgramScope(const string name) {
    Program* p = new Program(name, currentScope());
    global.defineProgram(*p);
    enterScope(p->scope());
    return *p;
}

void Symbols::leaveScope() {
    if (scopes.size() == 1) {
        throw runtime_error("Can't leave the global scope.");
    }
#ifdef DEBUG
    cout << "Leaving scope: " << *scopes.top();
#endif
    scopes.pop();
}

const Symbol* Symbols::resolve(const string name) const {
    return currentScope()->resolve(name);
}

const Type* Symbols::resolveType(const string name) const {
    return dynamic_cast<const Type*>(resolve(name));
}

const Variable* Symbols::resolveVariable(const string name) const {
    return dynamic_cast<const Variable*>(resolve(name));
}

const Method* Symbols::resolveMethod(const string name) const {
    return dynamic_cast<const Method*>(resolve(name));
}

const Class* Symbols::resolveClass(const string name) const {
    return dynamic_cast<const Class*>(resolve(name));
}

void Symbols::defineVariable(const string name, const Type &t) {
    Variable *v = new Variable(name, t);
    define(*v);
}

void Symbols::defineArray(const string name, const Type &t) {

    string typeName = ArrayType(t).name();
#ifdef DEBUG
    cout << "definning array var '" << name << "' of type " << typeName <<endl;
#endif
    const Type *at = resolveType(typeName);
    if (!at) {
        at = new const ArrayType(t);
        global.defineArrayAutoType(*dynamic_cast<const ArrayType*>(at));
#ifdef DEBUG
        cout << "new array type: " << *at << endl;
#endif
    }
    defineVariable(name, *at);
}

GlobalScope::GlobalScope() : Scope(NULL), _program(NULL) {
    const Type *mjInt = new Type("int");
    const Type *mjChar = new Type("char");
    const Type *mjArr = new AnyArrayType();
    const Variable *mjEol = new Variable("eol", *mjChar);
    const Variable *mjNull = new Variable("null", NULL_TYPE);

    // ord method
    MethodArguments *ordArgs = new MethodArguments(this);
    ordArgs->define(*(new Variable("c", *mjChar)));

    const MethodType *ordType = new MethodType(*ordArgs, *mjInt);
    
    Method *mjOrd = new Method("ord", *ordType);


    // chr method
    MethodArguments *chrArgs = new MethodArguments(this);
    chrArgs->define(*(new Variable("i", *mjInt)));

    const MethodType *chrType = new MethodType(*chrArgs, *mjChar);

    Method *mjChr = new Method("chr", *chrType);


    // len method
    MethodArguments *lenArgs = new MethodArguments(this);
    lenArgs->define(*(new Variable("arr", *mjArr)));

    const MethodType *lenType = new MethodType(*lenArgs, *mjInt);

    Method *mjLen = new Method("len", *lenType);

    define(*mjInt);
    define(*mjChar);
    define(VOID_TYPE);
    define(*mjEol);
    define(NULL_TYPE);
    define(*mjNull);
    define(*ordType);
    define(*mjOrd);
    define(*chrType);
    define(*mjChr);
    define(*lenType);
    define(*mjLen);
}

GlobalScope::~GlobalScope() {
    symbolTable.erase(VOID_TYPE.name());
    symbolTable.erase(NULL_TYPE.name());
    prototypes.clear();
}

void GlobalScope::defineArrayAutoType(const ArrayType &t) {
#ifdef DEBUG
    cout << "def arr type: " << t  << endl;
#endif
    Scope::define(t);
}

void GlobalScope::defineMethodAutoType(const MethodType &t) {
    Scope::define(t);
    prototypes.push_back(&t);
}

void GlobalScope::defineProgram(const Program &p) {
    if (_program) {
        throw runtime_error("Program already defined!" + _program->name());
    }
    Scope::define(p);
    _program = &p;
}

method_iterator GlobalScope::methodPrototypesBegin() {
    return prototypes.begin();
}

method_iterator GlobalScope::methodPrototypesEnd() {
    return prototypes.end();
}

const Program *GlobalScope::program() {
    return _program;
}

