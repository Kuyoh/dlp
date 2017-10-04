#include <iterator>
#include <fstream>
#include "DlpParser.hpp"
#include "AstPrinter.hpp"
#include "DlpLexer.hpp"
#include "parser.hpp"

using namespace dlp;

struct DlpParser::Data {
	std::unique_ptr<dlp::Parser> parser;
	std::unique_ptr<dlp::Lexer> scanner;
};

DlpParser::DlpParser() : data(new Data){
}

DlpParser::~DlpParser() {}

DlpParser::Program DlpParser::parse(const char *filename)
{
	std::ifstream in_file(filename);
	if (!in_file.good()) {
		std::cerr << "Failed to open file '" << filename << "'" << std::endl;
		exit(EXIT_FAILURE);
	}

	Program program; // = std::make_unique<Program>();
	data->scanner.reset(new Lexer(&in_file, &(program.filename)));
	data->parser.reset(new Parser(*data->scanner, &(program.code)));

	//if (l.begin.filename != nullptr)
	//	std::cerr << *(l.begin.filename) << "(" << l.begin.line << "," << l.begin.column << ") Error: " << err_message << std::endl;
	//else
	//	std::cerr << "Line " << l.begin.line << ", Column " << l.begin.column << " Error: " << err_message << std::endl;
	//*
	std::ofstream out_file("parsed.out");
	data->parser->set_debug_stream(out_file);
	data->parser->set_debug_level(1);
	/**/

	const int accept(0);
	if (data->parser->parse() != accept) {
		std::cerr << "Parse failed!" << std::endl;
		exit(EXIT_FAILURE);
	}
	//else
	//	std::cout << "Parse succeeded." << std::endl;

	/*
	std::ofstream out_file2("parsed_print.out");
	PrintVisitor v(out_file2);
	for (auto &stmt : *program.code)
		stmt->visit(v);
	/**/
	return std::move(program);
}
