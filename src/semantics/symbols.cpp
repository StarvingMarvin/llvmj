
#include "symbols.h"

using namespace mj;
using std::string;
using std::ostream;
using std::vector;
using std::map;
using std::auto_ptr;

Symbol::Symbol(const string name):
    _name(name)
{
}

ostream& operator<<(ostream &os, const Symbol& s) {
    return os << s.name();
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

void Scope::define(Symbol *s) {
    const string name = s->name();
    if (symbolTable[name] != NULL) {
        throw "Symbol already defined!";
    }
    symbolTable[name] = s;
}

const Symbol* Scope::resolve(const string name) {
    const Symbol *res = symbolTable[name];
    if (res == NULL) {
        if (_parent != NULL) {
            res = _parent->resolve(name);
        }
    }
    return res;
}

Variable::Variable(const string name, const Type& type):
    Symbol(name),
    _type(type)
{
}

MethodArguments::MethodArguments(Scope *parentScope): 
    Scope(parentScope), 
    arguments() 
{
}

bool MethodArguments::matchArguments(vector<Type*> argumentTypes) {
    return true;
}

Method::Method(const string name, 
        const Type returnType,
        Scope *parentScope):
    ScopeSymbol(name),
    _returnType(returnType),
    methodScope(new MethodScope(parentScope))
{
}

MethodScope::MethodScope (Scope *parent):
    Scope(parent)
{
}

Class::Class(string name, Scope *parentScope):
    Type(name),
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
    Type *mjInt = new Type("int");
    Type *mjChar = new Type("char");
    Type *mjVoid = new Type("void");

    Scope *global = new Scope(NULL);

    vector<Variable> ordArgs;
    //ordArgs.push_back(Variable("c", *mjChar));
    
    Method *mjOrd = new Method("ord", *mjInt, global);

    vector<Variable> chrArgs;
    //chrArgs.push_back(Variable("i", *mjInt));

    Method *mjChr = new Method("chr", *mjChar, global);

    global->define(mjInt);
    global->define(mjChar);
    global->define(mjVoid);
    //global->define(&NULL_TYPE);
    global->define(mjOrd);
    global->define(mjChr);
    //global->define(mjLen);

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

Class* Symbols::enterClassScope(const std::string name) {
    Class *c = new Class(name, currentScope());
    enterScope(c->scope());
    return c;
}

MethodArguments* Symbols::enterMethodArgumentsScope() {
    MethodArguments *ma = new MethodArguments(currentScope());
    enterScope(ma);
    return ma;
}

Method* Symbols::enterMethodScope(const std::string name, const Type returnType, MethodArguments * arguments) {
    Method* m = new Method(name, returnType, arguments);
    enterScope(m->scope());
    return m;
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

