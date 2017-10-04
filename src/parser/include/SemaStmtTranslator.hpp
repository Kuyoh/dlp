#ifndef DLP_PARSER_SEMAVISITOR_HPP
#define DLP_PARSER_SEMAVISITOR_HPP

#include "ast.hpp"
#include "AstVisitor.hpp"
#include "SemaTranslationContext.hpp"

namespace dlp {
	namespace sema {
		struct SemaStmtTranslator : ast::Visitor {
			SemaStmtTranslator();
			~SemaStmtTranslator();

			Type *translateType(ast::IType &type);
			void process(ast::StatementList &stmts);
			std::unique_ptr<Program> getProgram();
			
			void visitDefault() override;
			void visit(ast::Assignment &n) override;
			void visit(ast::Block &n) override;
			void visit(ast::ExternBlock &n) override;
			void visit(ast::IfStatement &n) override;
			void visit(ast::ForStatement &n) override;
			void visit(ast::ExpressionStatement &n) override;
			void visit(ast::ReturnStatement &n) override;
			void visit(ast::VariableDefinition &n) override;
			void visit(ast::Alias &n) override;
			void visit(ast::ConstantDefinition &n) override;

		protected:
			SemaTranslationContext context;
			struct Data;
			std::unique_ptr<Data> data;
		};
	}
}
#endif
