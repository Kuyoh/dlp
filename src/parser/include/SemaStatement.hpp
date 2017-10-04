#ifndef DLP_PARSER_SEMASTATEMENT_HPP
#define DLP_PARSER_SEMASTATEMENT_HPP

#include <list>
#include <memory>
#include "SemaExpression.hpp"
#include "SemaStatementVisitor.hpp"

namespace dlp {
	namespace sema {
		struct Statement {
			enum class Kind {
				BLOCK, CONDITIONAL, LOOP, RETURN, ASSIGNMENT, EXPRESSION
			};
			Kind getKind() const { return kind; }
			virtual ~Statement() = default;
			virtual void visit(SemaStatementVisitor &v) = 0;
		protected:
			Statement(Kind kind) : kind(kind) {}
			const Kind kind;
		};
		struct BlockStatement : Statement {
			BlockStatement() : Statement(Kind::BLOCK) {}
			void visit(SemaStatementVisitor &v) override { v.visit(*this); }
			static bool classof(const Statement *e) {
				return e->getKind() == Kind::BLOCK;
			}
			Scope *scope = nullptr;
		};
		struct ConditionalStatement : Statement {
			ConditionalStatement() : Statement(Kind::CONDITIONAL) {}
			void visit(SemaStatementVisitor &v) override { v.visit(*this); }
			static bool classof(const Statement *e) {
				return e->getKind() == Kind::CONDITIONAL;
			}

			Entity *expr = nullptr;
			Scope *thenBlock = nullptr;
			Scope *elseBlock = nullptr;
		};
		struct LoopStatement : Statement {
			LoopStatement() : Statement(Kind::LOOP) {}
			void visit(SemaStatementVisitor &v) override { v.visit(*this); }
			static bool classof(const Statement *e) {
				return e->getKind() == Kind::LOOP;
			}

			Entity *expr = nullptr;
			Scope *loopBlock = nullptr;
		};
		struct ReturnStatement : Statement {
			ReturnStatement() : Statement(Kind::RETURN) {}
			void visit(SemaStatementVisitor &v) override { v.visit(*this); }
			static bool classof(const Statement *e) {
				return e->getKind() == Kind::RETURN;
			}

			Entity *expr = nullptr;
		};
		struct AssignmentStatement : Statement {
			AssignmentStatement() : Statement(Kind::ASSIGNMENT) {}
			void visit(SemaStatementVisitor &v) override { v.visit(*this); }
			static bool classof(const Statement *e) {
				return e->getKind() == Kind::ASSIGNMENT;
			}

			Entity *lhs = nullptr;
			Entity *rhs = nullptr;
		};
		struct ExpressionStatement : Statement {
			ExpressionStatement() : Statement(Kind::EXPRESSION) {}
			void visit(SemaStatementVisitor &v) override { v.visit(*this); }
			static bool classof(const Statement *e) {
				return e->getKind() == Kind::EXPRESSION;
			}

			Entity *expr = nullptr;
		};
	}
}
#endif
