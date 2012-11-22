
#ifndef _SYMBOLS_H_
#define _SYMBOLS_H_

#include "llvmjConfig.h"

#include <string>
#include <vector>
#include <stack>
#include <map>
#include <cstdlib>
#include <memory>
#include <iostream>

namespace mj {

    class Printable {
        public:
            virtual std::ostream& print(std::ostream &os) const=0;
    };

    class Symbol : Printable
    {
        private:
            std::string _name;

        public:
            Symbol(const std::string name);
            const std::string name() const { return _name; }
            virtual std::ostream& print(std::ostream& os) const;
            virtual ~Symbol() {}
    };

    class Type : public Symbol
    {
        public:
            Type(const std::string name);
            virtual bool operator==(const Type &t) const;
            virtual bool operator!=(const Type &t) const;
            virtual bool compatible(const Type &t) const;
            virtual std::ostream& print(std::ostream& os)const;
    };

    const Type NULL_TYPE("mj.null");
    const Type VOID_TYPE("void");

    class ReferenceType: public Type {
        public:
            ReferenceType(const std::string name);
            virtual bool compatible(const Type &t) const;
    };

    class Variable : public Symbol {
        public:
            Variable(const std::string name, const Type& type);
            const Type& type() const { return _type; }
            virtual std::ostream& print(std::ostream& os)const;
        private:
            const Type& _type;
    };

    typedef std::map<const std::string, const Symbol*> SymbolTable;

    class Scope
    {
        public:
            Scope(Scope *parent);
            Scope *parent() const { return _parent; }
            virtual void define (const Symbol &s);
            virtual const Symbol* resolve(const std::string name);
            friend std::ostream& operator<<(std::ostream& os, const Scope& s);
            virtual ~Scope();
        protected:
            virtual unsigned int depth() const;
            SymbolTable symbolTable;

        private:
            Scope(Scope & s){}
            void operator=(Scope& s){}
            Scope *_parent;

    };

    class ScopeContainer : Printable {
        public:
            virtual Scope& scope()const=0;
            virtual std::ostream& print(std::ostream& os) const;
            virtual std::ostream& printSignature(std::ostream& os) const=0;
    };  

    class ArrayType: public ReferenceType {
        private:
            const Type &_valueType;
        public:
            ArrayType(const Type &valueType);
            const Type& valueType() const { return _valueType; }
    };

    class AnyArrayType: public ReferenceType {
        public:
            AnyArrayType(): ReferenceType("mj.array") {}
            virtual bool compatible(const Type& t) const { 
                return ReferenceType::compatible(t) 
                    || dynamic_cast<const ArrayType*>(&t) != NULL;
            }
    };

    typedef std::vector<const Type*> ArgumentTypes;
    
    class MethodArguments: public Scope {
        public:
            MethodArguments(Scope *parentScope);
            virtual void define(const Symbol &s);
            bool matchArguments(ArgumentTypes argumentTypes);
            std::string typeSignature();
        private:
            ArgumentTypes arguments;
    };

    class MethodScope: public Scope {
        public:
            MethodScope(Scope *parent);
        protected:
            virtual unsigned int depth() const { return Scope::depth() - 1; }
            /* virtual void define(Symbol *s);
            virtual Symbol *resolve(const std::string name); */
    };

    class MethodType: public Type {
        public:
            MethodType(MethodArguments &arguments, const Type &returnType);
            const Type &returnType() const { return _type; }
            MethodArguments& arguments() const { return _arguments; }
            virtual ~MethodType() { delete &_arguments; }
        private:
            const Type &_type;
            MethodArguments &_arguments;
    };

    class Method : public ScopeContainer, public Variable {
        friend class MethodScope;
        public:
            Method(const std::string name, 
                    const MethodType &methodType);
            virtual Scope& scope() const {return *methodScope;}
            virtual std::ostream& print(std::ostream& os) const;
            virtual std::ostream& printSignature(std::ostream& os) const;
            const MethodType& methodType() const { return dynamic_cast<const MethodType&>(type()); }
        private:
            MethodScope *methodScope;
    };

    class ClassScope : public Scope {
        public:
            ClassScope(Scope *parent, Type * c);
            virtual const Symbol *resolve(const std::string name);
            const Symbol *resolveField(const std::string name);
        private:
            Type *_c;
    };

    class Class : public ReferenceType, ScopeContainer {
        public:
            Class(std::string name, Scope *parentScope);
            virtual Scope& scope() const {return classScope;}
            virtual std::ostream& print(std::ostream& os) const;
            virtual std::ostream& printSignature(std::ostream& os) const;
            virtual ~Class() { delete &classScope; }
        private:
            ClassScope &classScope;
    };

    class Program : public ScopeContainer, public Symbol {
        public:
            Program(std::string name, Scope *parentScope);
            virtual Scope& scope() const {return _scope;}
            virtual std::ostream& print(std::ostream& os) const;
            virtual std::ostream& printSignature(std::ostream& os) const;
            virtual ~Program() { delete &_scope; }
        private:
            Scope &_scope;
    };

    class GlobalScope : public Scope {
        public:
        GlobalScope();
        virtual ~GlobalScope();
    };

    class Symbols {
        public:
            Symbols();

            const Symbol* resolve(const std::string name);
            const Type* resolveType(const std::string name);
            const Variable* resolveVariable(const std::string name);
            const Method* resolveMethod(const std::string name);
            const Class* resolveClass(const std::string name);

            void defineVariable(const std::string name, const Type &t);
            void defineArray(const std::string name, const Type &t);

            Class& enterClassScope(const std::string name);
            Program& enterProgramScope(const std::string name);
            Method& enterMethodScope(const std::string name, 
                    const Type &returnType, 
                    MethodArguments &arguments);
            MethodArguments& enterMethodArgumentsScope();
            void leaveScope();

            ~Symbols();

            friend std::ostream& operator<<(std::ostream& os, const Symbols& s);

        private:
            void define (Symbol &s);
            void enterScope(Scope& s) {scopes.push(&s);}
            Scope *currentScope() const {return scopes.top();}
            std::stack<Scope*> scopes;
    };

    std::ostream& operator<<(std::ostream &os, const mj::Symbol& s);

}


#endif //_SYMBOLS_H_

