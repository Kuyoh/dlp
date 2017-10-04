#ifndef DLP_PARSER_SEMATYPETRANSLATOR_HPP
#define DLP_PARSER_SEMATYPETRANSLATOR_HPP

#include "ast.hpp"
#include <memory>
#include "AstVisitor.hpp"
#include "SemaTranslationContext.hpp"

namespace dlp {
	namespace sema {
		struct SemaExprTranslator;
		struct SemaTypeTranslator : ast::Visitor {
			SemaTypeTranslator(SemaTranslationContext &context, SemaExprTranslator &expr);
			~SemaTypeTranslator();

			Type *translate(ast::IType &n);

			void visitDefault() override;
			void visit(ast::Identifier &n) override;
			void visit(ast::PointerType &n) override;
			void visit(ast::FunctionType &n) override;

		protected:
			SemaTranslationContext &context;
			struct Data;
			std::unique_ptr<Data> data;
		};
	}
}
#endif
