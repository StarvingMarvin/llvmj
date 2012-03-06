
#include "llvmjConfig.h"

#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Constants.h>
#include <llvm/GlobalVariable.h>
#include <llvm/Function.h>
#include <llvm/CallingConv.h>
#include <llvm/BasicBlock.h>
#include <llvm/Instructions.h>
#include <llvm/InlineAsm.h>
#include <llvm/Support/FormattedStream.h>
#include <llvm/Support/MathExtras.h>
#include <llvm/Pass.h>
#include <llvm/PassManager.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Assembly/PrintModulePass.h>
#include <algorithm>


using namespace llvm;

Module* makeLLVMModule();

int main(int argc, char**argv) {
  Module* Mod = makeLLVMModule();
  verifyModule(*Mod, PrintMessageAction);
  PassManager PM;
  PM.add(createPrintModulePass(&outs()));
  PM.run(*Mod);
  return 0;
}


Module* makeLLVMModule() {
  // Module Construction
  Module* mod = new Module("/tmp/webcompile/_2845_0.bc", getGlobalContext());
  mod->setDataLayout("e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64");
  mod->setTargetTriple("x86_64-unknown-linux-gnu");

  // Type Definitions
  ArrayType* ArrayTy_0 = ArrayType::get(IntegerType::get(mod->getContext(), 8), 4);

  PointerType* PointerTy_1 = PointerType::get(ArrayTy_0, 0);

  std::vector<const Type*>FuncTy_2_args;
  FuncTy_2_args.push_back(IntegerType::get(mod->getContext(), 32));
  FunctionType* FuncTy_2 = FunctionType::get(
    /*Result=*/IntegerType::get(mod->getContext(), 32),
    /*Params=*/FuncTy_2_args,
    /*isVarArg=*/false);

  PointerType* PointerTy_3 = PointerType::get(FuncTy_2, 0);

  std::vector<const Type*>FuncTy_4_args;
  FuncTy_4_args.push_back(IntegerType::get(mod->getContext(), 32));
  PointerType* PointerTy_6 = PointerType::get(IntegerType::get(mod->getContext(), 8), 0);

  PointerType* PointerTy_5 = PointerType::get(PointerTy_6, 0);

  FuncTy_4_args.push_back(PointerTy_5);
  FunctionType* FuncTy_4 = FunctionType::get(
    /*Result=*/IntegerType::get(mod->getContext(), 32),
    /*Params=*/FuncTy_4_args,
    /*isVarArg=*/false);

  std::vector<const Type*>FuncTy_8_args;
  FuncTy_8_args.push_back(PointerTy_6);
  FuncTy_8_args.push_back(PointerTy_5);
  FuncTy_8_args.push_back(IntegerType::get(mod->getContext(), 32));
  FunctionType* FuncTy_8 = FunctionType::get(
    /*Result=*/IntegerType::get(mod->getContext(), 64),
    /*Params=*/FuncTy_8_args,
    /*isVarArg=*/false);

  PointerType* PointerTy_7 = PointerType::get(FuncTy_8, 0);

  std::vector<const Type*>FuncTy_10_args;
  FuncTy_10_args.push_back(PointerTy_6);
  FunctionType* FuncTy_10 = FunctionType::get(
    /*Result=*/IntegerType::get(mod->getContext(), 32),
    /*Params=*/FuncTy_10_args,
    /*isVarArg=*/true);

  PointerType* PointerTy_9 = PointerType::get(FuncTy_10, 0);


  // Function Declarations

  Function* func_factorial = Function::Create(
    /*Type=*/FuncTy_2,
    /*Linkage=*/GlobalValue::ExternalLinkage,
    /*Name=*/"factorial", mod);
  func_factorial->setCallingConv(CallingConv::C);
  AttrListPtr func_factorial_PAL;
  {
    SmallVector<AttributeWithIndex, 4> Attrs;
    AttributeWithIndex PAWI;
    PAWI.Index = 4294967295U; PAWI.Attrs = 0  | Attribute::NoUnwind | Attribute::ReadNone;
    Attrs.push_back(PAWI);
    func_factorial_PAL = AttrListPtr::get(Attrs.begin(), Attrs.end());

  }
  func_factorial->setAttributes(func_factorial_PAL);

  Function* func_main = Function::Create(
    /*Type=*/FuncTy_4,
    /*Linkage=*/GlobalValue::ExternalLinkage,
    /*Name=*/"main", mod);
  func_main->setCallingConv(CallingConv::C);
  AttrListPtr func_main_PAL;
  {
    SmallVector<AttributeWithIndex, 4> Attrs;
    AttributeWithIndex PAWI;
    PAWI.Index = 2U; PAWI.Attrs = 0  | Attribute::NoCapture;
    Attrs.push_back(PAWI);
    PAWI.Index = 4294967295U; PAWI.Attrs = 0  | Attribute::NoUnwind;
    Attrs.push_back(PAWI);
    func_main_PAL = AttrListPtr::get(Attrs.begin(), Attrs.end());

  }
  func_main->setAttributes(func_main_PAL);

  Function* func_printf = Function::Create(
    /*Type=*/FuncTy_10,
    /*Linkage=*/GlobalValue::ExternalLinkage,
    /*Name=*/"printf", mod); // (external, no body)
  func_printf->setCallingConv(CallingConv::C);
  AttrListPtr func_printf_PAL;
  {
    SmallVector<AttributeWithIndex, 4> Attrs;
    AttributeWithIndex PAWI;
    PAWI.Index = 1U; PAWI.Attrs = 0  | Attribute::NoCapture;
    Attrs.push_back(PAWI);
    PAWI.Index = 4294967295U; PAWI.Attrs = 0  | Attribute::NoUnwind;
    Attrs.push_back(PAWI);
    func_printf_PAL = AttrListPtr::get(Attrs.begin(), Attrs.end());

  }
  func_printf->setAttributes(func_printf_PAL);

  Function* func_strtol = Function::Create(
    /*Type=*/FuncTy_8,
    /*Linkage=*/GlobalValue::ExternalLinkage,
    /*Name=*/"strtol", mod); // (external, no body)
  func_strtol->setCallingConv(CallingConv::C);
  AttrListPtr func_strtol_PAL;
  {
    SmallVector<AttributeWithIndex, 4> Attrs;
    AttributeWithIndex PAWI;
    PAWI.Index = 2U; PAWI.Attrs = 0  | Attribute::NoCapture;
    Attrs.push_back(PAWI);
    PAWI.Index = 4294967295U; PAWI.Attrs = 0  | Attribute::NoUnwind;
    Attrs.push_back(PAWI);
    func_strtol_PAL = AttrListPtr::get(Attrs.begin(), Attrs.end());

  }
  func_strtol->setAttributes(func_strtol_PAL);

  // Global Variable Declarations


  GlobalVariable* gvar_array__str = new GlobalVariable(/*Module=*/*mod,
  /*Type=*/ArrayTy_0,
  /*isConstant=*/true,
  /*Linkage=*/GlobalValue::PrivateLinkage,
  /*Initializer=*/0, // has initializer, specified below
  /*Name=*/".str");

  // Constant Definitions
  Constant* const_array_11 = ConstantArray::get(mod->getContext(), "%d\x0A", true);
  ConstantInt* const_int32_12 = ConstantInt::get(mod->getContext(), APInt(32, StringRef("0"), 10));
  ConstantInt* const_int32_13 = ConstantInt::get(mod->getContext(), APInt(32, StringRef("-1"), 10));
  ConstantInt* const_int32_14 = ConstantInt::get(mod->getContext(), APInt(32, StringRef("-2"), 10));
  ConstantInt* const_int32_15 = ConstantInt::get(mod->getContext(), APInt(32, StringRef("1"), 10));
  ConstantInt* const_int64_16 = ConstantInt::get(mod->getContext(), APInt(64, StringRef("1"), 10));
  ConstantPointerNull* const_ptr_17 = ConstantPointerNull::get(PointerTy_5);
  ConstantInt* const_int32_18 = ConstantInt::get(mod->getContext(), APInt(32, StringRef("10"), 10));
  std::vector<Constant*> const_ptr_19_indices;
  ConstantInt* const_int64_20 = ConstantInt::get(mod->getContext(), APInt(64, StringRef("0"), 10));
  const_ptr_19_indices.push_back(const_int64_20);
  const_ptr_19_indices.push_back(const_int64_20);
  Constant* const_ptr_19 = ConstantExpr::getGetElementPtr(gvar_array__str, &const_ptr_19_indices[0], const_ptr_19_indices.size());

  // Global Variable Definitions
  gvar_array__str->setInitializer(const_array_11);

  // Function Definitions

  // Function: factorial (func_factorial)
  {
    Function::arg_iterator args = func_factorial->arg_begin();
    Value* int32_X = args++;
    int32_X->setName("X");

    BasicBlock* label_entry = BasicBlock::Create(mod->getContext(), "entry",func_factorial,0);
    BasicBlock* label_if_end = BasicBlock::Create(mod->getContext(), "if.end",func_factorial,0);
    BasicBlock* label_if_end_i = BasicBlock::Create(mod->getContext(), "if.end.i",func_factorial,0);
    BasicBlock* label_factorial_exit = BasicBlock::Create(mod->getContext(), "factorial.exit",func_factorial,0);
    BasicBlock* label_return = BasicBlock::Create(mod->getContext(), "return",func_factorial,0);

    // Block entry (label_entry)
    ICmpInst* int1_cmp = new ICmpInst(*label_entry, ICmpInst::ICMP_EQ, int32_X, const_int32_12, "cmp");
    BranchInst::Create(label_return, label_if_end, int1_cmp, label_entry);

    // Block if.end (label_if_end)
    BinaryOperator* int32_sub = BinaryOperator::Create(Instruction::Add, int32_X, const_int32_13, "sub", label_if_end);
    ICmpInst* int1_cmp_i = new ICmpInst(*label_if_end, ICmpInst::ICMP_EQ, int32_sub, const_int32_12, "cmp.i");
    BranchInst::Create(label_factorial_exit, label_if_end_i, int1_cmp_i, label_if_end);

    // Block if.end.i (label_if_end_i)
    BinaryOperator* int32_sub_i = BinaryOperator::Create(Instruction::Add, int32_X, const_int32_14, "sub.i", label_if_end_i);
    CallInst* int32_call_i = CallInst::Create(func_factorial, int32_sub_i, "call.i", label_if_end_i);
    int32_call_i->setCallingConv(CallingConv::C);
    int32_call_i->setTailCall(true);AttrListPtr int32_call_i_PAL;
    {
      SmallVector<AttributeWithIndex, 4> Attrs;
      AttributeWithIndex PAWI;
      PAWI.Index = 4294967295U; PAWI.Attrs = 0  | Attribute::NoUnwind;
      Attrs.push_back(PAWI);
      int32_call_i_PAL = AttrListPtr::get(Attrs.begin(), Attrs.end());

    }
    int32_call_i->setAttributes(int32_call_i_PAL);

    BinaryOperator* int32_mul_i = BinaryOperator::Create(Instruction::Mul, int32_call_i, int32_sub, "mul.i", label_if_end_i);
    BranchInst::Create(label_factorial_exit, label_if_end_i);

    // Block factorial.exit (label_factorial_exit)
    PHINode* int32_call3 = PHINode::Create(IntegerType::get(mod->getContext(), 32), "call3", label_factorial_exit);
    int32_call3->reserveOperandSpace(2);
    int32_call3->addIncoming(int32_mul_i, label_if_end_i);
    int32_call3->addIncoming(const_int32_15, label_if_end);

    BinaryOperator* int32_mul = BinaryOperator::Create(Instruction::Mul, int32_call3, int32_X, "mul", label_factorial_exit);
    ReturnInst::Create(mod->getContext(), int32_mul, label_factorial_exit);

    // Block return (label_return)
    ReturnInst::Create(mod->getContext(), const_int32_15, label_return);

  }

  // Function: main (func_main)
  {
    Function::arg_iterator args = func_main->arg_begin();
    Value* int32_argc = args++;
    int32_argc->setName("argc");
    Value* ptr_argv = args++;
    ptr_argv->setName("argv");

    BasicBlock* label_entry_26 = BasicBlock::Create(mod->getContext(), "entry",func_main,0);
    BasicBlock* label_if_end_i_27 = BasicBlock::Create(mod->getContext(), "if.end.i",func_main,0);
    BasicBlock* label_if_end_i_i = BasicBlock::Create(mod->getContext(), "if.end.i.i",func_main,0);
    BasicBlock* label_factorial_exit_i = BasicBlock::Create(mod->getContext(), "factorial.exit.i",func_main,0);
    BasicBlock* label_factorial_exit_28 = BasicBlock::Create(mod->getContext(), "factorial.exit",func_main,0);

    // Block entry (label_entry_26)
    GetElementPtrInst* ptr_arrayidx = GetElementPtrInst::Create(ptr_argv, const_int64_16, "arrayidx", label_entry_26);
    LoadInst* ptr_tmp1 = new LoadInst(ptr_arrayidx, "tmp1", false, label_entry_26);
    std::vector<Value*> int64_call_i_params;
    int64_call_i_params.push_back(ptr_tmp1);
    int64_call_i_params.push_back(const_ptr_17);
    int64_call_i_params.push_back(const_int32_18);
    CallInst* int64_call_i = CallInst::Create(func_strtol, int64_call_i_params.begin(), int64_call_i_params.end(), "call.i", label_entry_26);
    int64_call_i->setCallingConv(CallingConv::C);
    int64_call_i->setTailCall(true);AttrListPtr int64_call_i_PAL;
    {
      SmallVector<AttributeWithIndex, 4> Attrs;
      AttributeWithIndex PAWI;
      PAWI.Index = 1U; PAWI.Attrs = 0  | Attribute::NoCapture;
      Attrs.push_back(PAWI);
      PAWI.Index = 4294967295U; PAWI.Attrs = 0  | Attribute::NoUnwind | Attribute::ReadOnly;
      Attrs.push_back(PAWI);
      int64_call_i_PAL = AttrListPtr::get(Attrs.begin(), Attrs.end());

    }
    int64_call_i->setAttributes(int64_call_i_PAL);

    CastInst* int32_conv_i = new TruncInst(int64_call_i, IntegerType::get(mod->getContext(), 32), "conv.i", label_entry_26);
    ICmpInst* int1_cmp_i_29 = new ICmpInst(*label_entry_26, ICmpInst::ICMP_EQ, int32_conv_i, const_int32_12, "cmp.i");
    BranchInst::Create(label_factorial_exit_28, label_if_end_i_27, int1_cmp_i_29, label_entry_26);

    // Block if.end.i (label_if_end_i_27)
    BinaryOperator* int32_sub_i_31 = BinaryOperator::Create(Instruction::Add, int32_conv_i, const_int32_13, "sub.i", label_if_end_i_27);
    ICmpInst* int1_cmp_i_i = new ICmpInst(*label_if_end_i_27, ICmpInst::ICMP_EQ, int32_sub_i_31, const_int32_12, "cmp.i.i");
    BranchInst::Create(label_factorial_exit_i, label_if_end_i_i, int1_cmp_i_i, label_if_end_i_27);

    // Block if.end.i.i (label_if_end_i_i)
    BinaryOperator* int32_sub_i_i = BinaryOperator::Create(Instruction::Add, int32_conv_i, const_int32_14, "sub.i.i", label_if_end_i_i);
    CallInst* int32_call_i_i = CallInst::Create(func_factorial, int32_sub_i_i, "call.i.i", label_if_end_i_i);
    int32_call_i_i->setCallingConv(CallingConv::C);
    int32_call_i_i->setTailCall(true);AttrListPtr int32_call_i_i_PAL;
    {
      SmallVector<AttributeWithIndex, 4> Attrs;
      AttributeWithIndex PAWI;
      PAWI.Index = 4294967295U; PAWI.Attrs = 0  | Attribute::NoUnwind;
      Attrs.push_back(PAWI);
      int32_call_i_i_PAL = AttrListPtr::get(Attrs.begin(), Attrs.end());

    }
    int32_call_i_i->setAttributes(int32_call_i_i_PAL);

    BinaryOperator* int32_mul_i_i = BinaryOperator::Create(Instruction::Mul, int32_call_i_i, int32_sub_i_31, "mul.i.i", label_if_end_i_i);
    BranchInst::Create(label_factorial_exit_i, label_if_end_i_i);

    // Block factorial.exit.i (label_factorial_exit_i)
    PHINode* int32_call3_i = PHINode::Create(IntegerType::get(mod->getContext(), 32), "call3.i", label_factorial_exit_i);
    int32_call3_i->reserveOperandSpace(2);
    int32_call3_i->addIncoming(int32_mul_i_i, label_if_end_i_i);
    int32_call3_i->addIncoming(const_int32_15, label_if_end_i_27);

    BinaryOperator* int32_mul_i_34 = BinaryOperator::Create(Instruction::Mul, int32_call3_i, int32_conv_i, "mul.i", label_factorial_exit_i);
    BranchInst::Create(label_factorial_exit_28, label_factorial_exit_i);

    // Block factorial.exit (label_factorial_exit_28)
    PHINode* int32_call24 = PHINode::Create(IntegerType::get(mod->getContext(), 32), "call24", label_factorial_exit_28);
    int32_call24->reserveOperandSpace(2);
    int32_call24->addIncoming(int32_mul_i_34, label_factorial_exit_i);
    int32_call24->addIncoming(const_int32_15, label_entry_26);

    std::vector<Value*> int32_call3_36_params;
    int32_call3_36_params.push_back(const_ptr_19);
    int32_call3_36_params.push_back(int32_call24);
    CallInst* int32_call3_36 = CallInst::Create(func_printf, int32_call3_36_params.begin(), int32_call3_36_params.end(), "call3", label_factorial_exit_28);
    int32_call3_36->setCallingConv(CallingConv::C);
    int32_call3_36->setTailCall(true);AttrListPtr int32_call3_36_PAL;
    {
      SmallVector<AttributeWithIndex, 4> Attrs;
      AttributeWithIndex PAWI;
      PAWI.Index = 4294967295U; PAWI.Attrs = 0  | Attribute::NoUnwind;
      Attrs.push_back(PAWI);
      int32_call3_36_PAL = AttrListPtr::get(Attrs.begin(), Attrs.end());

    }
    int32_call3_36->setAttributes(int32_call3_36_PAL);

    ReturnInst::Create(mod->getContext(), const_int32_12, label_factorial_exit_28);

  }

  return mod;
}
