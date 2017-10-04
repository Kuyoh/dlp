#ifndef DLP_PARSER_SEMASTATEMENTVISITOR_HPP
#define DLP_PARSER_SEMASTATEMENTVISITOR_HPP

namespace dlp {
    namespace sema {
		struct BlockStatement;
		struct ConditionalStatement;
		struct LoopStatement;
		struct ReturnStatement;
		struct AssignmentStatement;
		struct ExpressionStatement;
        struct SemaStatementVisitor {
            virtual ~SemaStatementVisitor() = default;
            virtual void visitDefault() {}
            virtual void visit(BlockStatement &n) { visitDefault(); }
            virtual void visit(ConditionalStatement &n) { visitDefault(); }
            virtual void visit(LoopStatement &n) { visitDefault(); }
            virtual void visit(ReturnStatement &n) { visitDefault(); }
            virtual void visit(AssignmentStatement &n) { visitDefault(); }
            virtual void visit(ExpressionStatement &n) { visitDefault(); }
        };
    }
}
#endif
