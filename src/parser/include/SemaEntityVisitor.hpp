#ifndef DLP_PARSER_SEMAENTITYVISITOR_HPP
#define DLP_PARSER_SEMAENTITYVISITOR_HPP

namespace dlp {
    namespace sema {
		struct Constant;
		struct LiteralInt;
		struct LiteralFloat;
        struct LiteralString;
		struct TypeType;
		struct BoolType;
		struct IntType;
		struct FloatType;
		struct StringType;
		struct FunctionType;
		struct PointerType;
		struct StructType;
		struct TypeSymbol;
		struct DependentType;

		struct OverloadedEntity;
		struct OverloadedFunction;

		struct Function;
		struct ImportedFunction;
		struct Symbol;
		struct CallExpr;
		struct MemAccessExpr;

        struct SemaEntityVisitor {
            virtual ~SemaEntityVisitor() = default;
            virtual void visitDefault() {}
            
			virtual void visit(Constant &n) { visitDefault(); }
			virtual void visit(LiteralInt &n) { visitDefault(); }
			virtual void visit(LiteralFloat &n) { visitDefault(); }
			virtual void visit(LiteralString &n) { visitDefault(); }

			virtual void visit(TypeType &n) { visitDefault(); }
			virtual void visit(BoolType &n) { visitDefault(); }
			virtual void visit(IntType &n) { visitDefault(); }
			virtual void visit(FloatType &n) { visitDefault(); }
			virtual void visit(StringType &n) { visitDefault(); }
			virtual void visit(FunctionType &n) { visitDefault(); }
			virtual void visit(PointerType &n) { visitDefault(); }
			virtual void visit(StructType &n) { visitDefault(); }
			virtual void visit(TypeSymbol &n) { visitDefault(); }
			virtual void visit(DependentType &n) { visitDefault(); }

			virtual void visit(Function &n) { visitDefault(); }
			virtual void visit(ImportedFunction &n) { visitDefault(); }
			
			virtual void visit(OverloadedEntity &n) { visitDefault(); }
			virtual void visit(OverloadedFunction &n) { visitDefault(); }
			virtual void visit(Symbol &n) { visitDefault(); }
			virtual void visit(CallExpr &n) { visitDefault(); }
			virtual void visit(MemAccessExpr &n) { visitDefault(); }
        };
    }
}
#endif
