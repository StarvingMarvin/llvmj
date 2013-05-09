
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
        Scope::iterator it = s.begin();
        for (; it != s.end(); ++it) {
            for (unsigned int d = 0; d <= depth; d++) os << "\t";
            os << *it << endl;
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


//
// Vars and consts
//
NamedValue::NamedValue(const string name, const Type& type):
    Symbol(name),
    _type(type)
{
}

ostream& NamedValue::print(ostream &os) const {
    return os << name() << " : " << type();
}

string escape(char c) {
    switch (c) {
        case '\n':
            return "\\n";
        case '\r':
            return "\\r";
        case '\t':
            return "\\t";
        default:
            return string(1, c);
    }
}

ostream& Constant::print(ostream &os) const {
    os << name() << " : " << type() << "("; 
    
    if (type().name() == "char") {
        os << "'" << escape((char)_value) << "'";
    } else {
        os << _value;
    }

    return os << ")";
}

//
// Scopes and scope symbols
//
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


ostream& ScopeContainer::print(ostream &os) const {
    printSignature(os);
    return os << this->scope();
}

//
// Method
//
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
    const NamedValue *v = dynamic_cast<const NamedValue*>(&s);
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
    NamedValue(name, type),
    methodScope(new MethodScope(&type.arguments()))
{
}

ostream& Method::printSignature(ostream &os) const {
    return NamedValue::print(os);
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

//
// Class
//
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

void SplitScope::define(const Symbol &s) {
    Scope::define(s);
    const Symbol *sp = &s;
    if (const Type *t = dynamic_cast<const Type*>(sp)) {
        types.push_back(t);
    } else if (const Method *m = dynamic_cast<const Method*>(sp)) {
        methods.push_back(m);
    } else if (const Class *c = dynamic_cast<const Class*>(sp)) {
        classes.push_back(c);
    } else if (const Constant *cc = dynamic_cast<const Constant*>(sp)) {
        constants.push_back(cc);
    } else if (const NamedValue *v = dynamic_cast<const NamedValue*>(sp)) {
        variables.push_back(v);
    }

}

//
// Program
//
Program::Program(string name, Scope *parentScope): 
    ScopeContainer(),
    Symbol(name),
    _scope(*(new SplitScope(parentScope)))
{
}

ostream& Program::printSignature(ostream &os) const {
    return Symbol::print(os);
}

ostream& Program::print(ostream &os) const {
    return ScopeContainer::print(os);
}

//
// Global Scope
//
GlobalScope::GlobalScope() : SplitScope(NULL), _program(NULL) {
    const Type *mjInt = new Type("int");
    const Type *mjChar = new Type("char");
    const Type *mjArr = new AnyArrayType();
    const Constant *mjEol = new Constant("eol", *mjChar, 10);
    const Constant *mjNull = new Constant("null", NULL_TYPE, 0);

    // ord method
    MethodArguments *ordArgs = new MethodArguments(this);
    ordArgs->define(*(new NamedValue("c", *mjChar)));

    const MethodType *ordType = new MethodType(*ordArgs, *mjInt);
    
    Method *mjOrd = new Method("ord", *ordType);


    // chr method
    MethodArguments *chrArgs = new MethodArguments(this);
    chrArgs->define(*(new NamedValue("i", *mjInt)));

    const MethodType *chrType = new MethodType(*chrArgs, *mjChar);

    Method *mjChr = new Method("chr", *chrType);


    // len method
    MethodArguments *lenArgs = new MethodArguments(this);
    lenArgs->define(*(new NamedValue("arr", *mjArr)));

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
    SplitScope::define(t);
}

void GlobalScope::defineMethodAutoType(const MethodType &t) {
    SplitScope::define(t);
    prototypes.push_back(&t);
}

void GlobalScope::defineProgram(const Program &p) {
    if (_program) {
        throw runtime_error("Program already defined!" + _program->name());
    }
    Scope::define(p);
    _program = &p;
}

//
// Symbols facade class
//
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

const NamedValue* Symbols::resolveNamedValue(const string name) const {
    return dynamic_cast<const NamedValue*>(resolve(name));
}

const Method* Symbols::resolveMethod(const string name) const {
    return dynamic_cast<const Method*>(resolve(name));
}

const Class* Symbols::resolveClass(const string name) const {
    return dynamic_cast<const Class*>(resolve(name));
}

void Symbols::defineNamedValue(const string name, const Type &t) {
    NamedValue *v = new NamedValue(name, t);
    define(*v);
}

void Symbols::defineConstant(const string name, const Type &t, int val) {
    Constant *c = new Constant(name, t, val);
    define(*c);
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
    defineNamedValue(name, *at);
}

