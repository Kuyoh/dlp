#pragma once
#include <string>
#include <llvm/IR/Module.h>
#include "DlpAnalyzer.hpp"

namespace dlp {
	constexpr char *ENTRY_FUNC_NAME = "__entry";

	//class DlpCodeGen {
	//public:
	//	DlpCodeGen();
	//	~DlpCodeGen();
	//
	//	void build(DlpAnalyzer::Program &ast, llvm::Module *module);
	//
	//protected:
	//	struct Data;
	//	Data *data;
	//};
}