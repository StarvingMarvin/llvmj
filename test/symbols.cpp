
#include "semantics/symbols.h"
#include <iostream>
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

}

int main (int argc, char** argv) {
    testTypes();
    std::cout << "All tests passed" << std::endl;
}

