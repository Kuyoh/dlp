#ifndef DLP_PARSER_ASTPRINTER_HPP
#define DLP_PARSER_ASTPRINTER_HPP

#include <iostream>
#include "AstVisitor.hpp"
#include "AstPrintingContext.hpp"

namespace dlp {
	struct PrintVisitor : ast::Visitor {
		PrintVisitor(std::ostream &o) : context(o) {}

		void visit(ast::StringLiteral &n) override;
		void visit(ast::HexLiteral &n) override;
		void visit(ast::OctalLiteral &n) override;
		void visit(ast::DecimalLiteral &n) override;
		void visit(ast::FloatLiteral &n) override;
		void visit(ast::Identifier &n) override;
		void visit(ast::MethodCall &n) override;
		void visit(ast::UnaryOperator &n) override;
		void visit(ast::BinaryOperator &n) override;
		void visit(ast::IndexAccess &n) override;
		void visit(ast::MemberAccess &n) override;
		void visit(ast::DefinitionExpression &n) override;
		void visit(ast::Assignment &n) override;
		void visit(ast::Block &n) override;
		void visit(ast::ExternBlock &n) override;
		void visit(ast::IfStatement &n) override;
		void visit(ast::ForStatement &n) override;
		void visit(ast::ExpressionStatement &n) override;
		void visit(ast::ReturnStatement &n) override;
		void visit(ast::VariableDefinition &n) override;
		void visit(ast::PointerType &n) override;
		void visit(ast::FunctionType &n) override;
		void visit(ast::Alias &n) override;
		void visit(ast::ConstantDefinition &n) override;

	protected:
		PrintingContext context;
	};
}
#endif
