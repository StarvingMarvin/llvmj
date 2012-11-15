
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

ostream& ScopeSymbol::print(ostream &os) const {
    printSignature(os);
    return os << *(this->scope());
}

ostream& ScopeSymbol::printSignature(ostream &os) const {
    return Symbol::print(os);
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

void MethodArguments::define(const Symbol *s) {
    const Variable *v = dynamic_cast<const Variable*>(s);
    if (v == NULL) {
        cerr << "ERROR! Expecting variable, got" << *s;
        throw "Illegal symbol";
    }
    Scope::define(v);  
    arguments.push_back(&v->type());
}

Method::Method(const string name, 
        const MethodType &type):
    ScopeSymbol(name),
    Variable(name, type),
    methodScope(new MethodScope(type.arguments()))
{
}

ostream& Method::printSignature(ostream &os) const {
    return Variable::print(os);
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

ostream& Class::printSignature(ostream &os) const {
    return Type::print(os);
}

ClassScope::ClassScope(Scope *parent, Symbol *c):
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

Program::Program(std::string name, Scope *parentScope): 
    ScopeSymbol(name),
    _scope(new Scope(parentScope))
{
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

//Scope* Symbols::enterNewScope() { 
//    Scope *s = new Scope(currentScope());
//    scopes.push(s);
//    return s;
//}

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

Method* Symbols::enterMethodScope(const std::string name, const Type &returnType, MethodArguments * arguments) {
    const MethodType *mtype = new MethodType(arguments, returnType);
    const Symbol *existingType = currentScope()->resolve(mtype->name());
    if (!existingType) {
        currentScope()->define(mtype);
#ifdef DEBUG
        std::cout << "new method type: " << *mtype << std::endl;
    } else {
        std::cout << "existing method type: " << *existingType << std::endl;
#endif
    }
    Method* m = new Method(name, *mtype);
    enterScope(m->scope());
    return m;
}

Program* Symbols::enterProgramScope(const std::string name) {
    Program* p = new Program(name, currentScope());
    enterScope(p->scope());
    return p;
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

