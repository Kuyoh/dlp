#ifndef DLP_PARSER_ASTSTATEMENT_HPP
#define DLP_PARSER_ASTSTATEMENT_HPP

#include "AstNode.hpp"
#include "AstExpression.hpp"

namespace dlp {
	namespace ast {
		using StatementList = UPtrList<IStatement>;
		using StringList = UPtrList<std::string>;

		struct VariableDefinition;
		struct VariableList : UPtrList<VariableDefinition> {
			bool isVarArg = false;
		};

		struct Assignment : IStatement {
			std::unique_ptr<std::string> lhs;
			std::unique_ptr<IExpression> rhs;
			Assignment(std::string *lhs, IExpression *rhs) :
				lhs(lhs), rhs(rhs) {
			}
			void visit(Visitor &v) override { v.visit(*this); }
		};

		struct Block : IBlock {
			StatementList statements;
			Block() {}
			Block(StatementList *stmts) : statements(std::move(*stmts)) {
				delete stmts;
			}
			void visit(Visitor &v) override { v.visit(*this); }
		};

		struct ExternBlock : IBlock {
			std::unique_ptr<IExpression> lib;
			std::unique_ptr<IExpression> func;
			ExternBlock() {}
			ExternBlock(IExpression *lib, IExpression *func) : lib(lib), func(func) {
			}
			void visit(Visitor &v) override { v.visit(*this); }
		};

		struct IfStatement : IStatement {
			std::unique_ptr<IExpression> expression;
			std::unique_ptr<IBlock> thenBlock;
			std::unique_ptr<IBlock> elseBlock;
			IfStatement(IExpression *expression, IBlock *thenBlock, IBlock *elseBlock = nullptr) :
				expression(expression), thenBlock(thenBlock), elseBlock(elseBlock) {
			}
			void visit(Visitor &v) override { v.visit(*this); }
		};

		struct ForStatement : IStatement {
			std::unique_ptr<IExpression> expression;
			std::unique_ptr<IBlock> block;
			ForStatement(IExpression *expression, IBlock *block) :
				expression(expression), block(block) {
			}
			void visit(Visitor &v) override { v.visit(*this); }
		};

		struct ExpressionStatement : IStatement {
			std::unique_ptr<IExpression> expression;
			ExpressionStatement(IExpression *expression) :
				expression(expression) {
			}
			void visit(Visitor &v) override { v.visit(*this); }
		};

		struct ReturnStatement : IStatement {
			std::unique_ptr<IExpression> expression;
			ReturnStatement(IExpression *expression) :
				expression(expression) {
			}
			void visit(Visitor &v) override { v.visit(*this); }
		};

		struct VariableDefinition : IStatement {
			std::unique_ptr<IType> type;
			std::unique_ptr<std::string> id;
			std::unique_ptr<IExpression> assignmentExpr;
			VariableDefinition(IType *type, std::string *id) :
				type(type), id(id), assignmentExpr(nullptr) {
			}
			VariableDefinition(IType *type, std::string *id, IExpression *assignmentExpr) :
				type(type), id(id), assignmentExpr(assignmentExpr) {
			}
			void visit(Visitor &v) override { v.visit(*this); }
		};
		
		struct PointerType : IType {
			std::unique_ptr<IType> type;
			PointerType(IType *type) : type(type) {
			}
			void visit(Visitor &v) override { v.visit(*this); }
		};
		struct FunctionType : IType {
			VariableList arguments;
			VariableList outArguments;
			FunctionType(VariableList *arguments) :
				arguments(std::move(*arguments)) {
				delete arguments;
			}
			FunctionType(VariableList *arguments, VariableList *outArguments) :
				arguments(std::move(*arguments)), outArguments(std::move(*outArguments)) {
				delete outArguments;
				delete arguments;
			}
			void visit(Visitor &v) override { v.visit(*this); }
		};

		struct Alias : IStatement {
			std::unique_ptr<std::string> id;
			std::unique_ptr<IType> type;
			Alias(std::string *id, IType *type) :
				id(id), type(type) {
			}
			void visit(Visitor &v) override { v.visit(*this); }
		};

		struct ConstantDefinition : IStatement {
			std::unique_ptr<std::string> id;
			std::unique_ptr<IExpression> expr;
			std::unique_ptr<IType> type;
			ConstantDefinition(std::string *id, IExpression *expr, IType *type = nullptr) :
				id(id), expr(expr), type(type) {
			}
			void visit(Visitor &v) override { v.visit(*this); }
		};
	}
}
#endif
