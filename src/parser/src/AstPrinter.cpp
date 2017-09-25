#include <iterator>
#include "AstPrinter.hpp"
#include "parser.hpp"

using namespace dlp;

void PrintVisitor::visit(ast::StringLiteral &n) {
	if (n.suffix)
		context.printNamedValue("STR", (*n.value + *n.suffix).c_str());
	else
		context.printNamedValue("STR", n.value->c_str());
}
void PrintVisitor::visit(ast::HexLiteral &n) {
	if (n.suffix)
		context.printNamedValue("HEXINT", (*n.value + *n.suffix).c_str());
	else
		context.printNamedValue("HEXINT", n.value->c_str());
}
void PrintVisitor::visit(ast::OctalLiteral &n) {
	if (n.suffix)
		context.printNamedValue("OCTINT", (*n.value + *n.suffix).c_str());
	else
		context.printNamedValue("OCTINT", n.value->c_str());
}
void PrintVisitor::visit(ast::DecimalLiteral &n) {
	if (n.suffix)
		context.printNamedValue("DECINT", (*n.value + *n.suffix).c_str());
	else
		context.printNamedValue("DECINT", n.value->c_str());
}
void PrintVisitor::visit(ast::FloatLiteral &n) {
	if (n.suffix)
		context.printNamedValue("FLOAT", (*n.value + *n.suffix).c_str());
	else
		context.printNamedValue("FLOAT", n.value->c_str());
}
void PrintVisitor::visit(ast::Identifier &n) { context.printNamedValue("ID", n.name->c_str()); }

void PrintVisitor::visit(ast::MethodCall &n) {
	context.startNode("CALL");
	context.printNamedValue("NAME", n.id->c_str());

	context.startNode("ARGUMENTS");
	for (auto &a : n.arguments)
		a->visit(*this);
	context.endNode();

	context.endNode();
}

void PrintVisitor::visit(ast::IndexAccess &n) {
	context.startNode("INDEX");

	context.startNode("AGGREGATE");
	n.aggregate->visit(*this);
	context.endNode();

	context.startNode("ARGUMENTS");
	for (auto &a : n.arguments)
		a->visit(*this);
	context.endNode();

	context.endNode();
}

void PrintVisitor::visit(ast::MemberAccess &n) {
	context.startNode("INDEX");

	context.startNode("AGGREGATE");
	n.aggregate->visit(*this);
	context.endNode();

	context.printNamedValue("NAME", n.memberName->c_str());

	context.endNode();

}

void PrintVisitor::visit(ast::UnaryOperator &n) {
	context.startNode("UNARY");
	context.printNamedValue("OPERATOR", unOpToken2Str(n.op));

	context.startNode("OPERAND");
	n.operand->visit(*this);
	context.endNode();

	context.endNode();
}

void PrintVisitor::visit(ast::BinaryOperator &n) {
	context.startNode("BINARY");
	context.printNamedValue("OPERATOR", binOpToken2Str(n.op));

	context.startNode("LEFT");
	n.lhs->visit(*this);
	context.endNode();

	context.startNode("RIGHT");
	n.rhs->visit(*this);
	context.endNode();

	context.endNode();
}


void PrintVisitor::visit(ast::DefinitionExpression &n) {
	context.startNode("DEFINITION");

	context.startNode("TYPE");
	n.type->visit(*this);
	context.endNode();

	n.block->visit(*this);

	context.endNode();
}

void PrintVisitor::visit(ast::Assignment &n) {
	context.startNode("ASSIGNMENT");
	context.printNamedValue("NAME", n.lhs->c_str());

	context.startNode("EXPRESSION");
	n.rhs->visit(*this);
	context.endNode();

	context.endNode();
}

void PrintVisitor::visit(ast::Block &n) {
	context.startNode("BLOCK");
	for (auto &a : n.statements)
		a->visit(*this);
	context.endNode();
}

void PrintVisitor::visit(ast::ExternBlock &n) {
	context.startNode("EXTERN");

	context.startNode("LIB");
	n.lib->visit(*this);
	context.endNode();

	context.startNode("FUNC");
	n.func->visit(*this);
	context.endNode();

	context.endNode();
}

void PrintVisitor::visit(ast::IfStatement &n) {
	context.startNode("IF");

	context.startNode("CONDITON");
	n.expression->visit(*this);
	context.endNode();

	context.startNode("THEN");
	n.thenBlock->visit(*this);
	context.endNode();

	context.startNode("ELSE");
	n.elseBlock->visit(*this);
	context.endNode();

	context.endNode();
}

void PrintVisitor::visit(ast::ForStatement &n) {
	context.startNode("FOR");

	context.startNode("CONDITON");
	n.expression->visit(*this);
	context.endNode();

	n.block->visit(*this);

	context.endNode();
}

void PrintVisitor::visit(ast::ExpressionStatement &n) {
	n.expression->visit(*this);
}

void PrintVisitor::visit(ast::ReturnStatement &n) {
	context.startNode("RETURN");

	context.startNode("EXPRESSION");
	n.expression->visit(*this);
	context.endNode();

	context.endNode();
}

void PrintVisitor::visit(ast::VariableDefinition &n) {
	context.startNode("VAR_DEF");
	context.printNamedValue("NAME", n.id->c_str());

	context.startNode("TYPE");
	if (n.type) n.type->visit(*this);
	context.endNode();

	context.startNode("VALUE");
	if (n.assignmentExpr) n.assignmentExpr->visit(*this);
	context.endNode();

	context.endNode();
}

void PrintVisitor::visit(ast::PointerType &n) {
	context.startNode("PTR_TYPE");
	n.type->visit(*this);
	context.endNode();
}

void PrintVisitor::visit(ast::FunctionType &n) {
	context.startNode("FUNC_TYPE");

	context.startNode("ARGUMENTS_IN");
	for (auto &a : n.arguments)
		a->visit(*this);
	context.endNode();

	context.startNode("ARGUMENTS_OUT");
	for (auto &a : n.outArguments)
		a->visit(*this);
	context.endNode();

	context.endNode();
}

void PrintVisitor::visit(ast::Alias &n) {
	context.startNode("ALIAS");
	context.printNamedValue("NAME", n.id->c_str());

	context.startNode("TYPE");
	n.type->visit(*this);
	context.endNode();

	context.endNode();
}

void PrintVisitor::visit(ast::ConstantDefinition &n) {
	context.startNode("CONSTANT");
	context.printNamedValue("NAME", n.id->c_str());

	context.startNode("EXPRESSION");
	n.expr->visit(*this);
	context.endNode();

	context.endNode();
}
