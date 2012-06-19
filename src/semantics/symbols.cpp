
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

Variable::Variable(const string name, const Type &type):
    Symbol(name),
    _type(type)
{
}

Method::Method(const string name, 
        const Type returnType):
    Symbol(name),
    _returnType(returnType)
{
}

MethodScope::MethodScope (Scope *parent):
    Scope(parent)
{
}

void MethodScope::define(Symbol *s) {
}

Symbol *MethodScope::resolve(const std::string name) {
}

Class::Class(string name):
    Type(name)
{
}

ClassScope::ClassScope(Scope *parent):
    Scope(parent)
{
}

void ClassScope::define(Symbol *s) {
    Scope::define(s);
}

const Symbol *ClassScope::resolve(const string name) {
    // don't look in parent scopes
    return symbolTable[name];
}

auto_ptr<Scope> mj::makeGlobalScope() {
    Type *mjInt = new Type("int");
    Type *mjChar = new Type("char");
    Type *mjVoid = new Type("void");

    vector<Variable> ordArgs;
    //ordArgs.push_back(Variable("c", *mjChar));
    
    Method *mjOrd = new Method("ord", *mjInt);

    vector<Variable> chrArgs;
    //chrArgs.push_back(Variable("i", *mjInt));

    Method *mjChr = new Method("chr", *mjChar);

    auto_ptr<Scope> global(new Scope(NULL));
    global->define(mjInt);
    global->define(mjChar);
    global->define(mjVoid);
    //global->define(&NULL_TYPE);
    global->define(mjOrd);
    global->define(mjChr);
    //global->define(mjLen);

    return global;
}

void Symbols::define(Symbol *s) {
    currentScope->define(s);
}

const Symbol* Symbols::resolve(string name) {
    return currentScope->resolve(name);
}

const Type* Symbols::resolveType(const string name) {
    return dynamic_cast<const Type*>(resolve(name));
}

const Variable* Symbols::resolveVariable(const string name) {
    return dynamic_cast<const Variable*>(resolve(name));
}
