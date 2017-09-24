#include "SemaEntityVisitor.hpp"
#include "SemaTranslationContext.hpp"
#include "SemaStatementVisitor.hpp"

namespace dlp {
    namespace sema {
        struct SemaNameResolver : SemaStatementVisitor, SemaEntityVisitor {
            SemaNameResolver(SemaTranslationContext &context);
            ~SemaNameResolver() override;

            void visit(Scope *s);
            void visit(BlockStatement &n) override;
            void visit(ConditionalStatement &n) override;
            void visit(LoopStatement &n) override;
            void visit(ReturnStatement &n) override;
            void visit(AssignmentStatement &n) override;
            void visit(ExpressionStatement &n) override;

			void visit(FunctionType &n) override;
			void visit(PointerType &n) override;
			void visit(StructType &n) override;
			void visit(TypeSymbol &n) override;
            void visit(DependentType &n) override;
            
			//void visit(Argument &n) override;
			//void visit(Variable &n) override;
			//void visit(Constant &n) override;
			void visit(Function &n) override;
			//void visit(ImportedFunction &n) override;
			//void visit(OverloadedFunction &n) override;

			// TODO: resolve OverloadedEntity into OverloadedFunction, possibly allow constants which then represent functions taking no argument and returning that constant
			
			// TODO: also for an definition make sure that the expression does not contain the variable, only a function body may reference the definition name
			// TODO: also verify order of symbol usage, it can not be used before it is defined!
			void visit(OverloadedEntity &n) override;


			void visit(Symbol &n) override;
			void visit(CallExpr &n) override;
            void visit(MemAccessExpr &n) override;
        protected:
            SemaTranslationContext &context;
            struct Data;
            std::unique_ptr<Data> data;
        };
    }
}