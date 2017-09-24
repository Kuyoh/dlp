#include <iterator>
#include <fstream>
#include "DlpAnalyzer.hpp"
#include "AstVisitor.hpp"
#include "SemaStmtTranslator.hpp"

using namespace dlp;
using namespace dlp::sema;

struct DlpAnalyzer::Data : ast::Visitor {
	SemaStmtTranslator translator;
	DlpAnalyzer::Program analyze(DlpParser::Program inputProgram);

	// Statements
	virtual void visit(ast::Assignment &n) { visitDefault(); }
	virtual void visit(ast::Block &n) { visitDefault(); }
	virtual void visit(ast::ExternBlock &n) { visitDefault(); }
	virtual void visit(ast::IfStatement &n) { visitDefault(); }
	virtual void visit(ast::ForStatement &n) { visitDefault(); }
	virtual void visit(ast::ExpressionStatement &n) { visitDefault(); }
	virtual void visit(ast::ReturnStatement &n) { visitDefault(); }
	virtual void visit(ast::VariableDefinition &n) { visitDefault(); }
	virtual void visit(ast::Alias &n) { visitDefault(); }
	virtual void visit(ast::ConstantDefinition &n) { visitDefault(); }
};

DlpAnalyzer::DlpAnalyzer() : data(new Data) {
}

DlpAnalyzer::~DlpAnalyzer() { }

DlpAnalyzer::Program DlpAnalyzer::analyze(DlpParser::Program inputProgram) { return data->analyze(std::move(inputProgram)); }

DlpAnalyzer::Program DlpAnalyzer::Data::analyze(DlpParser::Program inputProgram) {
	Program program;
	program.filename = inputProgram.filename;
	translator.process(*inputProgram.code);
	program.program = translator.getProgram();
	return std::move(program);
}
