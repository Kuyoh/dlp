#include "DlpJitEngine.hpp"
#include "DlpParser.hpp"
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/PassManager.h>
//#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>
#include <llvm/ADT/APFloat.h>
//#include <llvm/ADT/STLExtras.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>
#include <iostream>
#include "SemaAstPrinter.hpp"
#include "DlpJit.hpp"
#include "DlpParser.hpp"
#include "DlpCodeGen.hpp"

using namespace dlp;

struct DlpJitEngine::Data {
	DlpParser parser;
	DlpAnalyzer analyzer;
	DlpJit jit;
	//DlpCodeGen codeGen;
	llvm::LLVMContext context;
	std::unique_ptr<llvm::Module> workingModule;
	std::list<DlpJit::ModuleH> modules;
	//llvm::LLVMContext context;
	//llvm::Module module;
	//llvm::ModuleAnalysisManager analysisManager;
	
	//Data() : module("dlpjit", context) {}
	~Data() {
		for (auto &m : modules)
			jit.removeModule(m);
	}
};


DlpJitEngine::DlpJitEngine() : data(nullptr) {
	llvm::InitializeNativeTarget();
	llvm::InitializeNativeTargetAsmPrinter();
	llvm::InitializeNativeTargetAsmParser();
	data = new Data();
}
DlpJitEngine::~DlpJitEngine() {
	delete data;
}

void DlpJitEngine::add(const char *filename) {
	auto ast = data->analyzer.analyze(data->parser.parse(filename));
	
	printAst(std::cout, *ast.program);

	data->workingModule = llvm::make_unique<llvm::Module>("jit-module", data->context);
	data->workingModule->setDataLayout(data->jit.getDataLayout());

	//data->codeGen.build(ast, data->workingModule.get());

	data->modules.emplace_back(data->jit.addModule(std::move(data->workingModule)));
}


void DlpJitEngine::run() {
	
	//auto passManager = llvm::make_unique<llvm::ModulePassManager>();
	//
	//// Do simple "peephole" optimizations and bit-twiddling optzns.
	//passManager->addPass(llvm::createInstructionCombiningPass());
	//// Reassociate expressions.
	//passManager->addPass(llvm::createReassociatePass());
	//// Eliminate Common SubExpressions.
	//passManager->addPass(llvm::createGVNPass());
	//// Simplify the control flow graph (deleting unreachable blocks, etc).
	//passManager->addPass(llvm::createCFGSimplificationPass());
	//passManager.add(createPrintModulePass(&std::out));
	//
	//passManager->run(data->module, data->analysisManager);

	using MainFuncType = void(*)();
	MainFuncType mainFunc = (MainFuncType)data->jit.findSymbol(ENTRY_FUNC_NAME).getAddress().get(); //data->jit.findSymbol("main").getAddress();
	mainFunc();
}