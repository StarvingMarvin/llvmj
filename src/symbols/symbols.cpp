
#include "symbols.h"

using namespace mj;
using std::string;
using std::vector;
using std::auto_ptr;

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

