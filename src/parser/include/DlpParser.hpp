#pragma once
#include <memory>
#include "ast.hpp"

class DlpParser {
public:
	DlpParser();
	virtual ~DlpParser();

	struct Program {
		std::string filename;
		std::unique_ptr<dlp::ast::StatementList> code;
	};
	Program parse(const char *filename);

protected:
	struct Data;
	std::unique_ptr<Data> data;
};
