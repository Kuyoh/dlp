#include <iterator>
#include "AstPrintVisitor.hpp"
#include "parser.hpp"

using namespace dlp;

const char *PrintVisitor::unOpToken2Str(UnaryOperatorType token) {
	switch (token) {
	case UnaryOperatorType::MINUS: return "-";
	default: return "unknown_token";
	}
}

const char *PrintVisitor::binOpToken2Str(BinaryOperatorType token) {
	switch (token) {
	case BinaryOperatorType::COMP_EQ: return "==";
	case BinaryOperatorType::COMP_NEQ: return "!=";
	case BinaryOperatorType::COMP_LT: return "<";
	case BinaryOperatorType::COMP_LE: return "<=";
	case BinaryOperatorType::COMP_GT: return ">";
	case BinaryOperatorType::COMP_GE: return ">=";
	case BinaryOperatorType::PLUS: return "+";
	case BinaryOperatorType::MINUS: return "-";
	case BinaryOperatorType::MUL: return "*";
	case BinaryOperatorType::DIV: return "/";
	default: return "unknown_token";
	}
}

void PrintVisitor::writeIndent() {
	std::fill_n(std::ostream_iterator<char>(o), std::max(0, indent), '\t');
}

void PrintVisitor::startNode(const char *name) {
	writeIndent();
	o << name << " [" << std::endl;
	indent++;
}

void PrintVisitor::endNode() {
	indent--;
	writeIndent();
	o << "]" << std::endl;
}

void PrintVisitor::printNode(const char *name, ast::INode &n) {
	startNode(name);
	n.visit(*this);
	endNode();
}

void PrintVisitor::visit(ast::StringLiteral &n) {
	if (n.suffix)
		printValue("STR", (*n.value + *n.suffix).c_str());
	else
		printValue("STR", n.value->c_str());
}
void PrintVisitor::visit(ast::HexLiteral &n) {
	if (n.suffix)
		printValue("HEXINT", (*n.value + *n.suffix).c_str());
	else
		printValue("HEXINT", n.value->c_str());
}
void PrintVisitor::visit(ast::OctalLiteral &n) {
	if (n.suffix)
		printValue("OCTINT", (*n.value + *n.suffix).c_str());
	else
		printValue("OCTINT", n.value->c_str());
}
void PrintVisitor::visit(ast::DecimalLiteral &n) {
	if (n.suffix)
		printValue("DECINT", (*n.value + *n.suffix).c_str());
	else
		printValue("DECINT", n.value->c_str());
}
void PrintVisitor::visit(ast::FloatLiteral &n) {
	if (n.suffix)
		printValue("FLOAT", (*n.value + *n.suffix).c_str());
	else
		printValue("FLOAT", n.value->c_str()); 
}
void PrintVisitor::visit(ast::Identifier &n) { printValue("ID", n.name->c_str()); }

void PrintVisitor::visit(ast::MethodCall &n) {
	startNode("CALL");
	printValue("NAME", n.id->c_str());

	startNode("ARGUMENTS");
	for (auto &a : n.arguments)
		a->visit(*this);
	endNode();

	endNode();
}

void PrintVisitor::visit(ast::IndexAccess &n) {
	startNode("INDEX");

	startNode("AGGREGATE");
	n.aggregate->visit(*this);
	endNode();

	startNode("ARGUMENTS");
	for (auto &a : n.arguments)
		a->visit(*this);
	endNode();

	endNode();
}

void PrintVisitor::visit(ast::MemberAccess &n) {
	startNode("INDEX");

	startNode("AGGREGATE");
	n.aggregate->visit(*this);
	endNode();

	printValue("NAME", n.memberName->c_str());

	endNode();

}

void PrintVisitor::visit(ast::UnaryOperator &n) {
	startNode("UNARY");
	printValue("OPERATOR", unOpToken2Str(n.op));

	startNode("OPERAND");
	n.operand->visit(*this);
	endNode();

	endNode();
}

void PrintVisitor::visit(ast::BinaryOperator &n) {
	startNode("BINARY");
	printValue("OPERATOR", binOpToken2Str(n.op));

	startNode("LEFT");
	n.lhs->visit(*this);
	endNode();

	startNode("RIGHT");
	n.rhs->visit(*this);
	endNode();

	endNode();
}


void PrintVisitor::visit(ast::DefinitionExpression &n) {
	startNode("DEFINITION");

	startNode("TYPE");
	n.type->visit(*this);
	endNode();

	n.block->visit(*this);

	endNode();
}

void PrintVisitor::visit(ast::Assignment &n) {
	startNode("ASSIGNMENT");
	printValue("NAME", n.lhs->c_str());

	startNode("EXPRESSION");
	n.rhs->visit(*this);
	endNode();

	endNode();
}

void PrintVisitor::visit(ast::Block &n) {
	startNode("BLOCK");
	for (auto &a : n.statements)
		a->visit(*this);
	endNode();
}

void PrintVisitor::visit(ast::ExternBlock &n) {
	startNode("EXTERN");

	startNode("LIB");
	n.lib->visit(*this);
	endNode();

	startNode("FUNC");
	n.func->visit(*this);
	endNode();

	endNode();
}

void PrintVisitor::visit(ast::IfStatement &n) {
	startNode("IF");

	startNode("CONDITON");
	n.expression->visit(*this);
	endNode();

	startNode("THEN");
	n.thenBlock->visit(*this);
	endNode();

	startNode("ELSE");
	n.elseBlock->visit(*this);
	endNode();

	endNode();
}

void PrintVisitor::visit(ast::ForStatement &n) {
	startNode("FOR");

	startNode("CONDITON");
	n.expression->visit(*this);
	endNode();

	n.block->visit(*this);

	endNode();
}

void PrintVisitor::visit(ast::ExpressionStatement &n) {
	n.expression->visit(*this);
}

void PrintVisitor::visit(ast::ReturnStatement &n) {
	startNode("RETURN");

	startNode("EXPRESSION");
	n.expression->visit(*this);
	endNode();

	endNode();
}

void PrintVisitor::visit(ast::VariableDefinition &n) {
	startNode("VAR_DEF");
	printValue("NAME", n.id->c_str());

	startNode("TYPE");
	if (n.type) n.type->visit(*this);
	endNode();

	startNode("VALUE");
	if (n.assignmentExpr) n.assignmentExpr->visit(*this);
	endNode();

	endNode();
}

void PrintVisitor::visit(ast::PointerType &n) {
	startNode("PTR_TYPE");
	n.type->visit(*this);
	endNode();
}

void PrintVisitor::visit(ast::FunctionType &n) {
	startNode("FUNC_TYPE");

	startNode("ARGUMENTS_IN");
	for (auto &a : n.arguments)
		a->visit(*this);
	endNode();

	startNode("ARGUMENTS_OUT");
	for (auto &a : n.outArguments)
		a->visit(*this);
	endNode();

	endNode();
}

void PrintVisitor::visit(ast::Alias &n) {
	startNode("ALIAS");
	printValue("NAME", n.id->c_str());

	startNode("TYPE");
	n.type->visit(*this);
	endNode();

	endNode();
}

void PrintVisitor::visit(ast::ConstantDefinition &n) {
	startNode("CONSTANT");
	printValue("NAME", n.id->c_str());

	startNode("EXPRESSION");
	n.expr->visit(*this);
	endNode();

	endNode();
}
