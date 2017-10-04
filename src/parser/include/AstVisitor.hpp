#ifndef DLP_PARSER_ASTVISITOR_HPP
#define DLP_PARSER_ASTVISITOR_HPP

namespace dlp {
	namespace ast {
		struct INode;
		struct StringLiteral;
		struct HexLiteral;
		struct OctalLiteral;
		struct DecimalLiteral;
		struct FloatLiteral;
		struct Identifier;
		struct MethodCall;
		struct IndexAccess;
		struct MemberAccess;
		struct UnaryOperator;
		struct BinaryOperator;
		struct DefinitionExpression;
		struct Assignment;
		struct Block;
		struct ExternBlock;
		struct IfStatement;
		struct ForStatement;
		struct ExpressionStatement;
		struct ReturnStatement;
		struct VariableDefinition;
		struct PointerType;
		struct FunctionType;
		struct Alias;
		struct ConstantDefinition;

		struct Visitor {
			virtual void visitDefault() {}

			// Expressions
			virtual void visit(StringLiteral &) { visitDefault(); }
			virtual void visit(HexLiteral &) { visitDefault(); }
			virtual void visit(OctalLiteral &) { visitDefault(); }
			virtual void visit(DecimalLiteral &) { visitDefault(); }
			virtual void visit(FloatLiteral &) { visitDefault(); }
			virtual void visit(Identifier &) { visitDefault(); }
			virtual void visit(MethodCall &) { visitDefault(); }
			virtual void visit(IndexAccess &) { visitDefault(); }
			virtual void visit(MemberAccess &) { visitDefault(); }
			virtual void visit(UnaryOperator &) { visitDefault(); }
			virtual void visit(BinaryOperator &) { visitDefault(); }
			virtual void visit(DefinitionExpression &) { visitDefault(); }

			// Statements
			virtual void visit(Assignment &) { visitDefault(); }
			virtual void visit(Block &) { visitDefault(); }
			virtual void visit(ExternBlock &) { visitDefault(); }

			virtual void visit(IfStatement &) { visitDefault(); }
			virtual void visit(ForStatement &) { visitDefault(); }

			virtual void visit(ExpressionStatement &) { visitDefault(); }
			virtual void visit(ReturnStatement &) { visitDefault(); }
			virtual void visit(VariableDefinition &) { visitDefault(); }

			virtual void visit(Alias &) { visitDefault(); }
			virtual void visit(ConstantDefinition &) { visitDefault(); }

			// Types
			// virtual void visit(BasicType &) { visitDefault(); }// Replaced by Identifier
			virtual void visit(PointerType &) { visitDefault(); }
			virtual void visit(FunctionType &) { visitDefault(); }
		};
	}
}
#endif
