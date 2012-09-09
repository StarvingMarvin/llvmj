
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

    const Type NULL_TYPE("mj.null");
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

            virtual void define (const Symbol *s);

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

    class AnyArrayType: public Type {
        public:
            AnyArrayType(): Type("mj.array") {}
            virtual bool compatibile(const Type& t) const { return dynamic_cast<const ArrayType*>(&t) != NULL; }
    };
    
    class MethodArguments: public Scope {
        public:
            MethodArguments(Scope *parentScope);
            virtual void define(Symbol *s) { Scope::define(s);  arguments.push_back(s); }
            bool matchArguments(std::vector<const Type*> argumentTypes);
            std::string typeSignature();
        private:
            std::vector<Symbol*> arguments;
    };

    class MethodScope: public Scope {
        public:
            MethodScope(Scope *parent);
            /* virtual void define(Symbol *s);
            virtual Symbol *resolve(const std::string name); */
    };

    class MethodType: public Type {
        public:
            MethodType(MethodArguments *arguments, const Type &returnType);
            const Type &returnType() const { return _type; }
            MethodArguments* arguments() const { return _arguments; }
        private:
            const Type &_type;
            MethodArguments *_arguments;
    };

    class Method : public ScopeSymbol, public Variable {
        friend class MethodScope;
        public:
            Method(const std::string name, 
                    const MethodType &returnType);
            //const MethodType &type() const {return _type;}
            virtual Scope* scope() const {return methodScope;}
        private:
            //const MethodType &_type;
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
            Scope* enterNewScope();
            const Class* enterClassScope(const std::string name);
            const Method* enterMethodScope(const std::string name, const Type &returnType, MethodArguments* arguments);
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

