#include <iostream>
#include "llvm/ADT/STLExtras.h"
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include "llvm/ExecutionEngine/JITSymbol.h"
#include "llvm/ExecutionEngine/RTDyldMemoryManager.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/IRTransformLayer.h"
#include "llvm/ExecutionEngine/Orc/LambdaResolver.h"
#include <llvm/ExecutionEngine/OrcMCJITReplacement.h>
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/ExecutionEngine/RuntimeDyld.h"
#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/LazyEmittingLayer.h"
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Mangler.h"
#include "llvm/IR/IRPrintingPasses.h"
#include "llvm/Support/DynamicLibrary.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include <llvm/Support/TargetSelect.h>
#include <llvm/Object/Archive.h>
#include <functional>
#include <algorithm>
#include <memory>
#include <string>
#include <vector>

//#include <llvm/ADT/STLExtras.h>
//#include <llvm/ExecutionEngine/JITSymbol.h>
//#include <llvm/ExecutionEngine/ExecutionEngine.h>
//#include <llvm/ExecutionEngine/SectionMemoryManager.h>
//#include <llvm/ExecutionEngine/RTDyldMemoryManager.h>
//#include <llvm/ExecutionEngine/Orc/CompileUtils.h>
//#include <llvm/ExecutionEngine/Orc/IRCompileLayer.h>
//#include <llvm/ExecutionEngine/Orc/LambdaResolver.h>
//#include <llvm/ExecutionEngine/Orc/ObjectLinkingLayer.h>
//#include <llvm/IR/DataLayout.h>
////#include <llvm/Support/DynamicLibrary.h>
//#include <llvm/Target/TargetMachine.h>
////#include <llvm/Support/raw_ostream.h>
//#include <llvm/IR/Mangler.h>
#include "DlpJit.hpp"
#include <dynload.h>

using namespace dlp;

struct DlpRTLib {
	DLLib *lib;
	llvm::StringMap<void*> symbols;

	DlpRTLib(const llvm::DataLayout dl) {
		lib = dlLoadLibrary("dlprt.dll");
		auto *syms = dlSymsInit("dlprt.dll");
		int num = dlSymsCount(syms);
		for (int i = 0; i < num; ++i) {
			const char *symName = dlSymsName(syms, i);

			std::string mangledName;
			llvm::raw_string_ostream mangledNameStream(mangledName);
			llvm::Mangler::getNameWithPrefix(mangledNameStream, std::string("__") + symName, dl);
			symbols[mangledNameStream.str()] = dlFindSymbol(lib, symName);
		}
	}
	~DlpRTLib() {
		if (lib) dlFreeLibrary(lib);
	}
};

struct DlpJit::Data{
	//llvm::TargetMachine* tm;
	//llvm::ExecutionEngine *ee;
	std::unique_ptr<llvm::TargetMachine> tm;
	const llvm::DataLayout dl;
	DlpJit::ObjectLayer objectLayer;
	DlpJit::CompileLayer compileLayer;

	using OptimizeFunction =
		std::function<std::shared_ptr<llvm::Module>(std::shared_ptr<llvm::Module>)>;
	llvm::orc::IRTransformLayer<decltype(compileLayer), OptimizeFunction> optimizeLayer;

	DlpRTLib dlprtLib;

	std::shared_ptr<llvm::Module> optimize(std::shared_ptr<llvm::Module> m) {
		// Create a function pass manager.
		auto FPM = llvm::make_unique<llvm::legacy::FunctionPassManager>(m.get());

		// Add some optimizations.
		FPM->add(llvm::createInstructionCombiningPass());
		FPM->add(llvm::createReassociatePass());
		FPM->add(llvm::createGVNPass());
		FPM->add(llvm::createCFGSimplificationPass());

		FPM->doInitialization();

		// Run the optimizations over all functions in the module being added the JIT.
		for (auto &F : *m)
			FPM->run(F);

		//auto PM = llvm::make_unique<llvm::legacy::PassManager>();
		//llvm::raw_os_ostream os(std::cout);
		//PM->add(llvm::createPrintModulePass(os));
		//PM->run(*m);

		return std::move(m);
	}

	Data() : tm(llvm::EngineBuilder().selectTarget()), dl(tm->createDataLayout()),
		objectLayer([]() { return std::make_shared<llvm::SectionMemoryManager>(); }),
		compileLayer(objectLayer, llvm::orc::SimpleCompiler(*tm)), dlprtLib(dl),
		optimizeLayer(compileLayer, [this](std::shared_ptr<llvm::Module> m) { return optimize(std::move(m)); })
	{
	}
};

DlpJit::DlpJit() : data(new Data) {}
DlpJit::~DlpJit() { delete data; }

DlpJit::ModuleH DlpJit::addModule(std::unique_ptr<llvm::Module> m) {
	auto resolver = llvm::orc::createLambdaResolver(
		[=](const std::string &name) {
		if (auto Sym = data->optimizeLayer.findSymbol(name, false))
			return Sym;
		
		return llvm::JITSymbol(nullptr);
	},
	[=](const std::string &name) {
		// We could resolve external functions here, but we first need to create a lookup table for external symbols of the module.
		// Also the module probably needs to emit function code stating that the symbol is external

		//if (auto SymAddr =
		//	RTDyldMemoryManager::getSymbolAddressInProcess(Name))
		//	return JITSymbol(SymAddr, JITSymbolFlags::Exported);

		if (auto addr = data->dlprtLib.symbols.lookup(name))
			return llvm::JITSymbol(llvm::JITTargetAddress(addr), llvm::JITSymbolFlags::Exported);

		return llvm::JITSymbol(nullptr);
	});

	//data->objectLayer.
	//ee->addGlobalMapping(printfFcnt, libraries.get("msvcrt.dll").findSymbol("printf"));
	return llvm::cantFail(data->optimizeLayer.addModule(std::move(m), std::move(resolver)));
}

llvm::DataLayout DlpJit::getDataLayout() {
	return data->dl;
}

void DlpJit::removeModule(DlpJit::ModuleH m) {
	data->optimizeLayer.removeModule(m);
}

DlpJit::Symbol DlpJit::findSymbol(const std::string &name) {
	std::string mangledName;
	llvm::raw_string_ostream mangledNameStream(mangledName);
	llvm::Mangler::getNameWithPrefix(mangledNameStream, name, data->dl);
	return data->objectLayer.findSymbol(mangledNameStream.str(), false);
}
