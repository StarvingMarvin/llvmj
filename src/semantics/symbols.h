
#ifndef _SYMBOLS_H_
#define _SYMBOLS_H_

#include "llvmjConfig.h"

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

    class NamedValue : public Symbol {
        public:
            NamedValue(const std::string name, const Type& type);
            const Type& type() const { return _type; }
            virtual std::ostream& print(std::ostream& os)const;
        private:
            const Type& _type;
    };

    class Constant : public NamedValue {
        public:
            Constant(const std::string name, const Type &type, int val):
                NamedValue(name, type), _value(val) {}
            int value() { return _value; }
            virtual std::ostream& print(std::ostream& os)const;
        private:
            int _value;
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

            class iterator : 
                std::iterator<std::input_iterator_tag, Symbol>
            {
                public: 
                    iterator(SymbolTable::const_iterator it): _it(it){}

                    iterator& operator++() { _it++; return *this; }
                    
                    iterator operator++(int) 
                        { iterator tmp(*this); operator++(); return tmp; }

                    bool operator==(const iterator &rhs)
                        { return _it == rhs._it; }

                    bool operator!=(const iterator &rhs)
                        { return _it != rhs._it; }

                    const Symbol& operator*() { return *(_it->second); }
                private:
                    SymbolTable::const_iterator _it;
            };

            iterator begin() const { return iterator(symbolTable.begin()); }
            iterator end() const { return iterator(symbolTable.end()); }

        protected:
            virtual unsigned int depth() const;
            SymbolTable symbolTable;

        private:
            Scope(const Scope& s){}
            void operator=(const Scope& s){}
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
            explicit ArrayType(const Type &valueType);
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
            ArgumentTypes::const_iterator begin() const { return arguments.begin();}
            ArgumentTypes::const_iterator end() const {return arguments.end();}
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

    class Method : public ScopeContainer, public NamedValue {
        friend class MethodScope;
        public:
            Method(const std::string name, 
                    const MethodType &methodType);
            virtual Scope& scope() const {return *methodScope;}
            virtual std::ostream& print(std::ostream& os) const;
            virtual std::ostream& printSignature(std::ostream& os) const;
            const MethodType& methodType() const { return dynamic_cast<const MethodType&>(type()); }
            virtual ~Method() { delete methodScope; }
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

    typedef std::vector<const MethodType*>::const_iterator method_type_iterator;
    typedef std::vector<const ArrayType*>::const_iterator array_type_iterator;
    typedef std::vector<const Method*>::const_iterator method_iterator;
    typedef std::vector<const Constant*>::const_iterator constant_iterator;
    typedef std::vector<const NamedValue*>::const_iterator variable_iterator;
    typedef std::vector<const Class*>::const_iterator class_iterator;

    class SplitScope : public Scope {
        public:
            SplitScope(Scope *parentScope) : Scope(parentScope){}
            virtual void define(const Symbol &s); 
            method_iterator methodBegin() const { return methods.begin(); }
            method_iterator methodEnd() const { return methods.end(); }
            constant_iterator constantBegin() const { return constants.begin(); }
            constant_iterator constantEnd() const { return constants.end(); }
            variable_iterator variableBegin() const { return variables.begin(); }
            variable_iterator variableEnd() const { return variables.end(); }
            class_iterator classBegin() const { return classes.begin(); }
            class_iterator classEnd() const { return classes.end(); }
        private:
            std::vector<const Method*> methods;
            std::vector<const NamedValue*> variables;
            std::vector<const Constant*> constants;
            std::vector<const Class*> classes;
    };

    class Program : public ScopeContainer, public Symbol {
        public:
            Program(std::string name, Scope *parentScope);
            virtual Scope& scope() const {return _scope;}
            virtual std::ostream& print(std::ostream& os) const;
            virtual std::ostream& printSignature(std::ostream& os) const;
            virtual ~Program() { delete &_scope; }
        private:
            SplitScope &_scope;
    };

    class GlobalScope : public SplitScope {
        public:
            GlobalScope();
            void defineArrayAutoType(const ArrayType &t);
            void defineMethodAutoType(const MethodType &t);
            void defineProgram(const Program &p);
            
            method_type_iterator methodPrototypesBegin() const
                { return prototypes.begin(); }
            
            method_type_iterator methodPrototypesEnd() const
                { return prototypes.end(); }
            
            array_type_iterator arrayTypesBegin() const
                { return arrayTypes.begin(); }

            array_type_iterator arrayTypesEnd() const
                { return arrayTypes.end(); }


            const Program *program() const { return _program; }
            virtual ~GlobalScope();
        private:
            std::vector<const MethodType*> prototypes;
            std::vector<const ArrayType*> arrayTypes;
            const Program *_program;
    };

    class Symbols {
        public:
            Symbols();

            const Symbol* resolve(const std::string name) const;
            const Type* resolveType(const std::string name) const;
            const NamedValue* resolveNamedValue(const std::string name) const;
            const Method* resolveMethod(const std::string name) const;
            const Class* resolveClass(const std::string name) const;

            void defineNamedValue(const std::string name, const Type &t);
            void defineConstant(const std::string name, const Type &t, int vac);
            void defineArray(const std::string name, const Type &t);

            Class& enterClassScope(const std::string name);
            Program& enterProgramScope(const std::string name);
            Method& enterMethodScope(const std::string name, 
                    const Type &returnType, 
                    MethodArguments &arguments);
            MethodArguments& enterMethodArgumentsScope();
            void leaveScope();
            const GlobalScope &globalScope() const { return global; }
            // no destructor as global will cleanup allocates symbols

            friend std::ostream& operator<<(std::ostream& os, const Symbols& s);

        private:
            void define (const Symbol &s);
            void enterScope(Scope& s) {scopes.push(&s);}
            Scope *currentScope() const {return scopes.top();}
            std::stack<Scope*> scopes;
            GlobalScope global;
    };

    std::ostream& operator<<(std::ostream &os, const mj::Symbol& s);

}


#endif //_SYMBOLS_H_

