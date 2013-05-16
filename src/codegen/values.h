#ifndef VALUES_H
#define VALUES_H

namespace mj {

typedef std::map<const std::string, llvm::Value*> ValueTable;
typedef std::map<const std::string, llvm::Type*> TypeTable;
typedef std::map<const std::string, int> IndexTable;

class Values {
public:
    Values(llvm::Module* module, const mj::Symbols &symbols);
    llvm::Value* value(const std::string &name) const;
    llvm::Type* type(const std::string &name) const;
    void define(std::string name, llvm::Value *value);
    int index(const std::string &structName, const std::string &fieldName) const;
    void enterScope(ValueTable &local);
    void leaveScope();

private:

    void initPrimitives(const mj::GlobalScope &global);
    void initTypes(const mj::GlobalScope &global);
    llvm::Type *initClass(const mj::Class &c);
    llvm::Type *initArrayType(const mj::ArrayType &at);
    void initGlobals(const mj::GlobalScope &global);
    void initMethods(const mj::GlobalScope &global);

    llvm::Module* _module;

    ValueTable globalValues;
    ValueTable *localScope;
    TypeTable types;
    IndexTable fieldIndices;
};

}

#endif // VALUES_H
