
#include "semantics/symbols.h"
#include <iostream>
#include <string>
#include <cassert>

using namespace mj;

void testTypes() {
    // Simple types
    Type my_int("int");
    Type my_char("char");
    Type my_other_int("int");

    assert(my_int == my_other_int);
    assert(my_int != my_char);
    assert(!my_char.compatible(my_int));

    ArrayType int_array(my_int);
    ArrayType other_int_array(my_other_int);
    ArrayType char_array(my_char);
    AnyArrayType aa;

    assert(int_array == other_int_array);
    assert(int_array.compatible(other_int_array));
    assert(!int_array.compatible(char_array));
    assert(char_array.compatible(NULL_TYPE));
    assert(aa.compatible(NULL_TYPE));
    assert(aa.compatible(char_array));
    assert(!aa.compatible(my_int));
}

void testMethod() {
    const Type *mjInt = new Type("int");
    const Type *mjChar = new Type("char");

    Scope *global = new Scope(NULL);

    // ord method
    MethodArguments *ordArgs = new MethodArguments(global);
    ordArgs->define(*(new Variable("c", *mjChar)));

    const MethodType *ordType = new MethodType(*ordArgs, *mjInt);
    assert(ordType->returnType() == *mjInt);
    
    const Method *mjOrd = new Method("ord", *ordType);

    // toUpper method
    MethodArguments *upperArgs = new MethodArguments(global);
    upperArgs->define(*(new Variable("c", *mjChar)));

    const MethodType *upperType = new MethodType(*upperArgs, *mjChar);
    assert(upperType->returnType() == *mjChar);
    
    const Method *upper = new Method("toUpper", *upperType);

    global->define(*mjInt);
    global->define(*mjChar);
    global->define(*ordType);
    global->define(*dynamic_cast<const Variable*>(mjOrd));
    global->define(*upperType);
    global->define(*dynamic_cast<const Variable*>(upper));

    const Symbol *ots = global->resolve(ordType->name());
    assert(std::string("(char)->int") == ots->name());
    assert(dynamic_cast<const MethodType*>(ots)->returnType() == *mjInt);


    const Symbol *uts = global->resolve(upperType->name());
    assert(std::string("(char)->char") == uts->name());
    assert(dynamic_cast<const MethodType*>(uts)->returnType() == *mjChar);

    const Symbol *upperSym = global->resolve("toUpper");
    const Variable *upperVar = dynamic_cast<const Variable*>(upperSym);
    const Type &upperVarType = upperVar->type();
    const MethodType &upperVarMethodType = dynamic_cast<const MethodType&>(upperVarType);
    assert(upperVarMethodType.returnType() == *mjChar);

}

int main (int argc, char** argv) {
    testTypes();
    testMethod();
    std::cout << "All tests passed" << std::endl;
}

