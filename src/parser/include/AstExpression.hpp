#ifndef DLP_PARSER_ASTEXPRESSION_HPP
#define DLP_PARSER_ASTEXPRESSION_HPP

#include "AstNode.hpp"
#include "OperatorTypes.hpp"

namespace dlp {
	namespace ast {
		using ExpressionList = UPtrList<IExpression>;

		struct Identifier : virtual IExpression, virtual IType {
			std::unique_ptr<std::string> name;
			Identifier(std::string *name) : name(name) {};
			void visit(Visitor &v) override { v.visit(*this); }
		};

		struct StringLiteral : IExpression {
			std::unique_ptr<std::string> value;
			std::unique_ptr<std::string> suffix;
			StringLiteral(std::string *value, std::string *suffix = nullptr) : value(value), suffix(suffix) {}
			void visit(Visitor &v) override { v.visit(*this); }
		};

		struct HexLiteral : IExpression {
			std::unique_ptr<std::string> value;
			std::unique_ptr<std::string> suffix;
			HexLiteral(std::string *value, std::string *suffix = nullptr) : value(value), suffix(suffix) {}
			void visit(Visitor &v) override { v.visit(*this); }
		};

		struct OctalLiteral : IExpression {
			std::unique_ptr<std::string> value;
			std::unique_ptr<std::string> suffix;
			OctalLiteral(std::string *value, std::string *suffix = nullptr) : value(value), suffix(suffix) {}
			void visit(Visitor &v) override { v.visit(*this); }
		};

		struct DecimalLiteral : IExpression {
			std::unique_ptr<std::string> value;
			std::unique_ptr<std::string> suffix;
			DecimalLiteral(std::string *value, std::string *suffix = nullptr) : value(value), suffix(suffix) {}
			void visit(Visitor &v) override { v.visit(*this); }
		};

		struct FloatLiteral : IExpression {
			std::unique_ptr<std::string> value;
			std::unique_ptr<std::string> suffix;
			FloatLiteral(std::string *value, std::string *suffix = nullptr) : value(value), suffix(suffix) {}
			void visit(Visitor &v) override { v.visit(*this); }
		};

		struct MethodCall : IExpression {
			std::unique_ptr<std::string> id;
			ExpressionList arguments;
			MethodCall(std::string *id, ExpressionList *arguments) :
				id(id), arguments(std::move(*arguments)) {
				delete arguments;
			}
			MethodCall(std::string *id) : id(id) {}
			void visit(Visitor &v) override { v.visit(*this); }
		};

		struct IndexAccess : IExpression {
			std::unique_ptr<IExpression> aggregate;
			ExpressionList arguments;
			IndexAccess(IExpression *aggregate, ExpressionList *arguments) :
				aggregate(aggregate), arguments(std::move(*arguments)) {
				delete arguments;
			}
			void visit(Visitor &v) override { v.visit(*this); }
		};

		struct MemberAccess : IExpression {
			std::unique_ptr<IExpression> aggregate;
			std::unique_ptr<std::string> memberName;
			MemberAccess(IExpression *aggregate, std::string *memberName) :
				aggregate(aggregate), memberName(memberName) {
			}
			void visit(Visitor &v) override { v.visit(*this); }
		};

		struct UnaryOperator : IExpression {
			UnaryOperatorType op;
			std::unique_ptr<IExpression> operand;
			UnaryOperator(IExpression *operand, UnaryOperatorType op) :
				operand(operand), op(op) {
			}
			void visit(Visitor &v) override { v.visit(*this); }
		};

		struct BinaryOperator : IExpression {
			BinaryOperatorType op;
			std::unique_ptr<IExpression> lhs;
			std::unique_ptr<IExpression> rhs;
			BinaryOperator(IExpression *lhs, BinaryOperatorType op, IExpression *rhs) :
				lhs(lhs), rhs(rhs), op(op) {
			}
			void visit(Visitor &v) override { v.visit(*this); }
		};

		struct DefinitionExpression : IExpression {
			std::unique_ptr<IType> type;
			std::unique_ptr<IBlock> block;
			DefinitionExpression(IType *type, IBlock *block) :
				type(type), block(block) {
			}
			void visit(Visitor &v) override { v.visit(*this); }
		};
	}
}
#endif
