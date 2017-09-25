#include "SemaAstPrinter.hpp"
#include "AstPrintingContext.hpp"

using namespace dlp;
using namespace dlp::sema;

struct StatementPrinter;
struct EntityPrinter : SemaEntityVisitor {
	StatementPrinter *sp;
	PrintingContext &context;
	EntityPrinter(PrintingContext &context) : context(context) {}

	void visitDefault() override { }

	void printEntity(const char *name, Entity &e);

	void visit(Argument &n) override;
	void visit(Variable &n) override;

	void visit(Constant &n) override;
	void visit(LiteralInt &n) override;
	void visit(LiteralFloat &n) override;
	void visit(LiteralString &n) override;

	void visit(TypeType &n) override;
	void visit(BoolType &n) override;
	void visit(IntType &n) override;
	void visit(FloatType &n) override;
	void visit(StringType &n) override;
	void visit(FunctionType &n) override;
	void visit(PointerType &n) override;
	void visit(StructType &n) override;
	void visit(TypeSymbol &n) override;
	void visit(DependentType &n) override;

	void visit(Function &n) override;
	void visit(ImportedFunction &n) override;

	void visit(OverloadedEntity &n) override;
	void visit(OverloadedFunction &n) override;
	void visit(Symbol &n) override;
	void visit(CallExpr &n) override;
	void visit(MemAccessExpr &n) override;
};

struct StatementPrinter : SemaStatementVisitor {
	EntityPrinter *ep;
	PrintingContext &context;
	StatementPrinter(PrintingContext &context) : context(context) {}

	void visitDefault() override { }

	void visit(Scope &s);
	void visit(BlockStatement &n) override;
	void visit(ConditionalStatement &n) override;
	void visit(LoopStatement &n) override;
	void visit(ReturnStatement &n) override;
	void visit(AssignmentStatement &n) override;
	void visit(ExpressionStatement &n) override;
};


// printAst impl
void dlp::printAst(std::ostream &o, sema::Program &p) {
	PrintingContext context(o);
	StatementPrinter stmt(context);
	EntityPrinter entity(context);
	stmt.ep = &entity;
	entity.sp = &stmt;
	stmt.visit(p);
}

// StatementPrinter Impl
void StatementPrinter::visit(Scope &s) {
	context.startNode("SCOPE");
	for (auto &s : s.statements)
		s->visit(*this);
	context.endNode();
}
void StatementPrinter::visit(BlockStatement &n) {
	context.startNode("BLOCK");
	for (auto &s : n.scope->statements)
		s->visit(*this);
	context.endNode();
}
void StatementPrinter::visit(ConditionalStatement &n) {
	context.startNode("CONDITONAL");

	ep->printEntity("CONDITION", *n.expr);

	context.startNode("THEN");
	visit(*n.thenBlock);
	context.endNode();

	if (n.elseBlock != nullptr) {
		context.startNode("ELSE");
		visit(*n.elseBlock);
		context.endNode();
	}

	context.endNode();
}
void StatementPrinter::visit(LoopStatement &n) {
	context.startNode("LOOP");
	ep->printEntity("EXPRESSION", *n.expr);
	visit(*n.loopBlock);
	context.endNode();
}
void StatementPrinter::visit(ReturnStatement &n) {
	ep->printEntity("RETURN", *n.expr);
}
void StatementPrinter::visit(AssignmentStatement &n) {
	context.startNode("ASSIGNMENT");
	ep->printEntity("LHS", *n.lhs);
	ep->printEntity("RHS", *n.rhs);
	context.endNode();
}
void StatementPrinter::visit(ExpressionStatement &n) {
	ep->printEntity("EXPRSTMT", *n.expr);
}

// EntityPrinter Impl
void EntityPrinter::printEntity(const char *name, Entity &e) {
	context.startNode(name);
	e.visit(*this);
	context.endNode();
}

void EntityPrinter::visit(Argument &n) {
	context.startNode("ARG");
	context.printNamedValue("NAME", n.name.c_str());
	if (n.type != nullptr)
		context.printNamedValue("TYPE", n.type);
	context.endNode();
}
void EntityPrinter::visit(Variable &n) {
	context.startNode("VAR");
	context.printNamedValue("NAME", n.name.c_str());
	if (n.type != nullptr)
		context.printNamedValue("TYPE", n.type);
	context.endNode();
}

void EntityPrinter::visit(Constant &n) {
	context.startNode("CONSTANT");
	context.printNamedValue("NAME", n.name.c_str());
	if (n.type != nullptr)
		context.printNamedValue("TYPE", n.type);
	printEntity("EXPR", *n.expr);
	context.endNode();
}
void EntityPrinter::visit(LiteralInt &n) {
	context.startNode("LITERAL");
	context.printNamedValue("VALUE", n.value);
	if (n.type != nullptr)
		context.printNamedValue("TYPE", n.type);
	context.endNode();
}
void EntityPrinter::visit(LiteralFloat &n) {
	context.startNode("LITERAL");
	context.printNamedValue("VALUE", n.value);
	if (n.type != nullptr)
		context.printNamedValue("TYPE", n.type);
	context.endNode();
}
void EntityPrinter::visit(LiteralString &n) {
	context.startNode("LITERAL");
	context.printNamedValue("VALUE", n.value.c_str());
	if (n.type != nullptr)
		context.printNamedValue("TYPE", n.type);
	context.endNode();
}

void EntityPrinter::visit(TypeType &n) {
	context.printValue("TYPE");
}
void EntityPrinter::visit(BoolType &n) {
	context.printValue("BOOL");
}
void EntityPrinter::visit(IntType &n) {
	std::string typeName = n.isSigned ? "INT" : "UINT";
	switch (n.bitness) {
	case Type::B8: typeName += "8"; break;
	case Type::B16: typeName += "16"; break;
	case Type::B32: typeName += "32"; break;
	case Type::B64: typeName += "64"; break;
	case Type::B128: typeName += "128"; break;
	default: return;
	}
	context.printValue(typeName.c_str());
}
void EntityPrinter::visit(FloatType &n) {
	context.printValue(n.bitness == Type::B32 ? "FLOAT" : "DOUBLE");
}
void EntityPrinter::visit(StringType &n) {
	context.printValue("STRING");
}
void EntityPrinter::visit(FunctionType &n) {
	context.startNode("FUNCTIONTYPE");

	context.printNamedValue("ISVARARG", n.isVarArg ? "true" : "false");

	context.startNode("ARGUMENTS");
	for (auto &a : n.arguments) {
		context.startNode(a.name.c_str());
		if (a.type != nullptr)
			printEntity("TYPE", *a.type);
		if (a.defaultVal != nullptr)
			printEntity("DEFAULT", *a.defaultVal);
		context.endNode();
	}
	context.endNode();

	context.startNode("RESULTS");
	for (auto &a : n.results) {
		context.startNode(a.name.c_str());
		if (a.type != nullptr)
			printEntity("TYPE", *a.type);
		if (a.defaultVal != nullptr)
			printEntity("DEFAULT", *a.defaultVal);
		context.endNode();
	}
	context.endNode();

	context.endNode();
}
void EntityPrinter::visit(PointerType &n) {
	context.startNode("POINTER");
	n.elementType->visit(*this);
	context.endNode();
}
void EntityPrinter::visit(StructType &n) {
	context.startNode("STRUCT");
	for (auto &a : n.attributes) {
		context.startNode(a.name.c_str());
		if (a.type != nullptr)
			printEntity("TYPE", *a.type);
		context.endNode();
	}
	context.endNode();
}
void EntityPrinter::visit(TypeSymbol &n) {
	context.startNode("TYPESYMBOL");
	context.printNamedValue("NAME", n.name.c_str());
	if (n.resolvedType != nullptr)
		printEntity("RESOLVEDTYPE", *n.resolvedType);
	context.endNode();
}
void EntityPrinter::visit(DependentType &n) {
	context.startNode("DEPENDENTTYPE");
	printEntity("DEPENDENCY", *n.dependency);
	if (n.type != nullptr)
		printEntity("RESOLVEDTYPE", *n.type);
	context.endNode();
}

void EntityPrinter::visit(Function &n) {
	context.printValue("FUNC");
}
void EntityPrinter::visit(ImportedFunction &n) {
	context.printValue("EXTERNFUNC");
	//context.startNode("EXTERNFUNC");
	//printEntity("LIBNAME", *n.libName);
	//printEntity("FUNCNAME", *n.funcName);
	//context.endNode();
}

void EntityPrinter::visit(OverloadedEntity &n) { context.printValue("OVERLOADEDENTITY"); }
void EntityPrinter::visit(OverloadedFunction &n) { context.printValue("OVERLOADEDFUNC"); }
void EntityPrinter::visit(Symbol &n) {
	context.startNode("SYMBOL");

	context.printNamedValue("NAME", n.name.c_str());
	if (n.entity != nullptr)
		printEntity("VALUE", *n.entity);
	if (n.type != nullptr)
		printEntity("RESOLVEDTYPE", *n.type);

	context.endNode();
}
void EntityPrinter::visit(CallExpr &n) {
	context.startNode("CALL");

	context.printNamedValue("FUNCNAME", n.funcName.c_str());

	if (n.funcEntity != nullptr)
		printEntity("RESOLVEDFUNC", *n.funcEntity);

	context.startNode("ARGUMENTS");
	for (auto *a : n.arguments)
		printEntity("ARG", *a);
	context.endNode();

	context.endNode();
}
void EntityPrinter::visit(MemAccessExpr &n) {
	context.startNode("MEMACCESS");

	printEntity("OBJECT", *n.object);

	context.printNamedValue("MEMBER", n.memName.c_str());

	if (n.type != nullptr)
		printEntity("RESOLVEDTYPE", *n.type);

	context.endNode();
}

