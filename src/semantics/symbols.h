
#ifndef _SYMBOLS_H_
#define _SYMBOLS_H_

#include <string>
#include <vector>
#include <stack>
#include <map>
#include <cstdlib>
#include <memory>

namespace mj {

    class Symbol
    {
        private:
            std::string _name;

        public:

            Symbol(const std::string name);

            virtual const std::string name() const { return _name; }

    };

    class Type : public Symbol
    {
        public:
            Type(const std::string name);
            virtual bool operator==(const Type &t) const;
            virtual bool operator!=(const Type &t) const;
            virtual bool compatible(const Type &t) const;
    };

    const Type NULL_TYPE("null");
    const Type VOID_TYPE("void");
    const Type UNDEFINED_TYPE("MJ.undefined");

    class ReferenceType: public Type {
        public:
            ReferenceType(const std::string name);
            bool compatible(const Type &t) const;
    };

    class Variable : public Symbol {
        public:
            Variable(const std::string name, const Type& type);
            const Type& type() const { return _type; }
        private:
            const Type& _type;
    };

    class Scope
    {
        public:
            Scope(Scope *parent);

            Scope *parent() const { return _parent; }

            virtual void define (Symbol *s);

            virtual const Symbol* resolve(const std::string name);

        protected:
            std::map<const std::string, const Symbol*> symbolTable;

        private:
            Scope(Scope & s){}
            void operator=(Scope& s){}
            Scope *_parent;

    };

    class ScopeSymbol : public Symbol {
        public:
            ScopeSymbol(const std::string name): Symbol(name){};
            virtual Scope* scope()const=0;
    };

    class ArrayType: public ReferenceType {
        private:
            const Type &_valueType;
        public:
            ArrayType(const Type &valueType);
            const Type& valueType() const { return _valueType; }
    };

    class MethodArguments: public Scope {
        public:
            MethodArguments(Scope *parentScope);
            virtual void define(Symbol *s) { Scope::define(s);  arguments.push_back(s); }
            bool matchArguments(std::vector<Type*> argumentTypes);
        private:
            std::vector<Symbol*> arguments;
    };

    class MethodScope: public Scope {
        public:
            MethodScope(Scope *parent);
            /* virtual void define(Symbol *s);
            virtual Symbol *resolve(const std::string name); */
    };

    class Method : public ScopeSymbol {
        friend class MethodScope;
        public:
            Method(const std::string name, 
                    const Type returnType,
                    Scope * parentScope);
            const Type returnType() const {return _returnType;}
            virtual Scope* scope() const {return methodScope;}
        private:
            const Type _returnType;
            std::vector<Variable*> _arguments;
            MethodScope *methodScope;
    };

    class ClassScope : public Scope {
        public:
            ClassScope(Scope *parent, Symbol * c);
            virtual void define(Symbol *s);
            virtual const Symbol *resolve(const std::string name);
            const Symbol *resolveField(const std::string name);
        private:
            Symbol *_c;
    };

    class Class : public Type, ScopeSymbol {
        friend class ClassScope;
        public:
            Class(std::string name, Scope *parentScope);
            virtual Scope* scope() const {return classScope;}
        private:
            std::vector<Variable> _fields;
            ClassScope *classScope;
    };

    class Symbols {
        public:
            Symbols();

            void define (Symbol *s);

            const Symbol* resolve(const std::string name);
            const Type* resolveType(const std::string name);
            const Variable* resolveVariable(const std::string name);

            void enterScope(Scope* s) {scopes.push(s);}
            Class* enterClassScope(const std::string name);
            Method* enterMethodScope(const std::string name, const Type returnType, MethodArguments* arguments);
            MethodArguments* enterMethodArgumentsScope();
            void leaveScope() {scopes.pop();}

        private:
            Scope *currentScope() {return scopes.top();}
            std::stack<Scope*> scopes;
    };

    Scope *makeGlobalScope();

}

std::ostream& operator<<(std::ostream &os, const mj::Symbol& s);

#endif //_SYMBOLS_H_

