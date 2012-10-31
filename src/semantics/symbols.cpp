
#include "symbols.h"

using namespace mj;
using std::string;
using std::ostream;
using std::vector;
using std::map;
using std::auto_ptr;
using std::cerr;
using std::endl;

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
        os << "{\n";
        SymbolTable::const_iterator i = s.symbolTable.begin();
        for (; i != s.symbolTable.end(); i++) {
            os << "\t" << *(i->second) << endl;
        }
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

void Scope::define(const Symbol *s) {
    const string name = s->name();
    if (symbolTable[name] != NULL) {
        throw "Symbol already defined!";
    }
    symbolTable[name] = s;
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

Variable::Variable(const string name, const Type& type):
    Symbol(name),
    _type(type)
{
}

ostream& Variable::print(ostream &os) const {
    return os << name() << " : " << type();
}

ostream& ScopeSymbol::print(ostream &os) const {
    Symbol::print(os);
    return os << *(this->scope());
}

MethodArguments::MethodArguments(Scope *parentScope): 
    Scope(parentScope), 
    arguments() 
{
}

bool MethodArguments::matchArguments(vector<const Type*> argumentTypes) {
    return true;
}

string MethodArguments::typeSignature() {
    string sig = "(";
    string sep = "";
    for (vector<Symbol*>::iterator it = arguments.begin(); it != arguments.end(); ++it) {
        sig += sep;
        sig += (*it)->name();
        sep = ",";
    }
    sig += ")";
    return sig;
}

Method::Method(const string name, 
        const MethodType &type):
    ScopeSymbol(name),
    Variable(name, type),
    methodScope(new MethodScope(type.arguments()))
{
}

MethodScope::MethodScope (Scope *parent):
    Scope(parent)
{
}

MethodType::MethodType(MethodArguments *arguments, const Type &returnType):
    Type(arguments->typeSignature() + "->" + returnType.name()),
    _type(returnType),
    _arguments(arguments)
{
}

Class::Class(string name, Scope *parentScope):
    ReferenceType(name),
    ScopeSymbol(name)
{
    classScope = new ClassScope(parentScope, dynamic_cast<Type*>(this));
}

ClassScope::ClassScope(Scope *parent, Symbol *c):
    Scope(parent),
    _c(c)
{
}

void ClassScope::define(Symbol *s) {
    Scope::define(s);
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

Scope *mj::makeGlobalScope() {
    const Type *mjInt = new Type("int");
    const Type *mjChar = new Type("char");
    const Type *mjVoid = new Type("void");
    const Type *mjArr = new AnyArrayType();
    const Variable *mjEol = new Variable("eol", *mjChar);
    const Variable *mjNull = new Variable("null", NULL_TYPE);

    Scope *global = new Scope(NULL);

    // ord method
    MethodArguments *ordArgs = new MethodArguments(global);
    ordArgs->define(new Variable("c", *mjChar));

    const MethodType *ordType = new MethodType(ordArgs, *mjInt);
    
    Method *mjOrd = new Method("ord", *ordType);


    // chr method
    MethodArguments *chrArgs = new MethodArguments(global);
    chrArgs->define(new Variable("i", *mjInt));

    const MethodType *chrType = new MethodType(chrArgs, *mjChar);

    Method *mjChr = new Method("chr", *chrType);


    // len method
    MethodArguments *lenArgs = new MethodArguments(global);
    lenArgs->define(new Variable("arr", *mjArr));

    const MethodType *lenType = new MethodType(lenArgs, *mjInt);

    Method *mjLen = new Method("len", *lenType);

    global->define(mjInt);
    global->define(mjChar);
    global->define(mjVoid);
    global->define(mjEol);
    global->define(&NULL_TYPE);
    global->define(mjNull);
    global->define(ordType);
    global->define(dynamic_cast<const ScopeSymbol*>(mjOrd));
    global->define(chrType);
    global->define(dynamic_cast<const ScopeSymbol*>(mjChr));
    global->define(lenType);
    global->define(dynamic_cast<const ScopeSymbol*>(mjLen));

    return global;
}

Symbols::Symbols():
    scopes(std::stack<Scope*>())
{
    scopes.push(makeGlobalScope());
}

void Symbols::define(Symbol *s) {
    currentScope()->define(s);
}

Scope* Symbols::enterNewScope() { 
    Scope *s = new Scope(currentScope());
    scopes.push(s);
    return s;
}

const Class* Symbols::enterClassScope(const std::string name) {
    const Class *c = new Class(name, currentScope());
    enterScope(c->scope());
    return c;
}

MethodArguments* Symbols::enterMethodArgumentsScope() {
    MethodArguments *ma = new MethodArguments(currentScope());
    enterScope(ma);
    return ma;
}

const Method* Symbols::enterMethodScope(const std::string name, const Type &returnType, MethodArguments * arguments) {
    const MethodType *mtype = new MethodType(arguments, returnType);
    if (! currentScope()->resolve(mtype->name())) {
        currentScope()->define(mtype);
    }
    const Method* m = new Method(name, *mtype);
    enterScope(m->scope());
    return m;
}

void Symbols::leaveScope() {
#ifdef DEBUG
    std::cout << "Leaving scope: " << *scopes.top();
#endif
    scopes.pop();
}

const Symbol* Symbols::resolve(string name) {
    return currentScope()->resolve(name);
}

const Type* Symbols::resolveType(const string name) {
    return dynamic_cast<const Type*>(resolve(name));
}

const Variable* Symbols::resolveVariable(const string name) {
    return dynamic_cast<const Variable*>(resolve(name));
}

