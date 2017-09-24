#pragma once
#include <memory>
#include "ast.hpp"
#include "DlpParser.hpp"
#include "SemaProgram.hpp"

class DlpAnalyzer {
public:
	DlpAnalyzer();
	virtual ~DlpAnalyzer();

	struct Program {
		std::string filename;
		std::unique_ptr<dlp::sema::Program> program;
		// statements, definitions...
	};
	Program analyze(DlpParser::Program program);

protected:
	struct Data;
	std::unique_ptr<Data> data;
};
