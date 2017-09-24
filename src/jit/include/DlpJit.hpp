#pragma once
#include <string>
#include <llvm/IR/Module.h>
#include <llvm/ExecutionEngine/Orc/IRCompileLayer.h>
#include <llvm/ExecutionEngine/Orc/ObjectLinkingLayer.h>

// see http://llvm.org/docs/tutorial/LangImpl04.html
// and https://llvm.org/docs/tutorial/BuildingAJIT1.html

namespace dlp {
	class DlpJit {
	public:
		using Symbol = llvm::JITSymbol;
		using ModuleH = llvm::orc::IRCompileLayer<llvm::orc::ObjectLinkingLayer<>>::ModuleSetHandleT;

		DlpJit();
		virtual ~DlpJit();

		ModuleH addModule(std::unique_ptr<llvm::Module> m);
		void removeModule(ModuleH m);

		llvm::DataLayout getDataLayout();

		// maybe we just call run() instead of findSymbol?
		Symbol findSymbol(const std::string &name);

	private:
		struct Data;
		Data *data;
	};
}