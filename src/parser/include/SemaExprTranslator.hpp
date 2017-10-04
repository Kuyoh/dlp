#ifndef DLP_PARSER_SEMAEXPRTRANSLATOR_HPP
#define DLP_PARSER_SEMAEXPRTRANSLATOR_HPP

#include <memory>
#include "ast.hpp"
#include "AstVisitor.hpp"
#include "SemaTypeTranslator.hpp"

namespace dlp {
	namespace sema {
		struct SemaStmtTranslator;
		struct SemaExprTranslator : ast::Visitor {
			SemaExprTranslator(SemaTranslationContext &context, SemaStmtTranslator &stmt);
			~SemaExprTranslator();

			Entity *translate(ast::IExpression &n);

			void visitDefault() override;
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

		protected:
			SemaTranslationContext &context;		
			Entity *translateCall(std::string name, std::vector<Entity*> args);			
			struct Data;
			std::unique_ptr<Data> data;
		};
	}
}
#endif
