#include "SemaStmtTranslator.hpp"
#include "SemaExprTranslator.hpp"
#include "SemaTypeTranslator.hpp"

using namespace dlp;
using namespace dlp::sema;

struct SemaStmtTranslator::Data {
	SemaTypeTranslator type;
	SemaExprTranslator expr;
	// TODO: we need to fix init-order here!
	// => translateType func in semastmttranslator
	Data(SemaStmtTranslator &stmt, SemaTranslationContext &context) : expr(context, stmt), type(context, expr) {}
};

SemaStmtTranslator::SemaStmtTranslator() : data(std::make_unique<Data>(*this, context)) {
}
SemaStmtTranslator::~SemaStmtTranslator() {}

Type *SemaStmtTranslator::translateType(ast::IType &type) {
	return data->type.translate(type);
}

void SemaStmtTranslator::process(ast::StatementList &stmts) {
	for (auto &s : stmts)
		s->visit(*this);
}

std::unique_ptr<Program> SemaStmtTranslator::getProgram() {
	return std::move(context.program);
}

void SemaStmtTranslator::visitDefault() {
#ifdef _MSC_VER	
	__debugbreak();
#endif
}

void SemaStmtTranslator::visit(ast::Assignment &n) {
	auto *stmt = context.addStatement<AssignmentStatement>();
	stmt->rhs = data->expr.translate(*n.rhs);
	stmt->lhs = context.create<Symbol>(*n.lhs);
}

void SemaStmtTranslator::visit(ast::Block &n) {
	process(n.statements);
	// start phase 2 here?
}

void SemaStmtTranslator::visit(ast::ExternBlock &n) {
	visitDefault();
}

void SemaStmtTranslator::visit(ast::IfStatement &n) {
	auto *stmt = context.addStatement<ConditionalStatement>();
	stmt->expr = data->expr.translate(*n.expression);
	stmt->thenBlock = context.pushScope();
	n.thenBlock->visit(*this);
	context.popScope();
	if (n.elseBlock) {
		stmt->elseBlock = context.pushScope();
		n.elseBlock->visit(*this);
		context.popScope();
	}
}

void SemaStmtTranslator::visit(ast::ForStatement &n) {
	auto *stmt = context.addStatement<LoopStatement>();
	stmt->expr = data->expr.translate(*n.expression);
	stmt->loopBlock = context.pushScope();
	n.block->visit(*this);
	context.popScope();
}

void SemaStmtTranslator::visit(ast::ExpressionStatement &n) {
	auto *stmt = context.addStatement<ExpressionStatement>();
	stmt->expr = data->expr.translate(*n.expression);
}

void SemaStmtTranslator::visit(ast::ReturnStatement &n) {
	auto *stmt = context.addStatement<ReturnStatement>();
	stmt->expr = data->expr.translate(*n.expression);
}

// StatementList
void SemaStmtTranslator::visit(ast::VariableDefinition &n) {
	auto &slot = context.nameEntity(*n.id);
	if (slot.entity)
		context.logError("redefinition of entity");
	else {
		auto *v = context.create<Variable>(*n.id);
		slot.entity = v;
		Entity *rhs = nullptr;
		if (n.assignmentExpr) {
			auto *as = context.addStatement<AssignmentStatement>();
			as->lhs = v;
			rhs = as->rhs = data->expr.translate(*n.assignmentExpr);
		}
		if (n.type)
			v->type = data->type.translate(*n.type);
		else
			v->type = context.create<DependentType>(rhs);
	}
}

void SemaStmtTranslator::visit(ast::Alias &n) {
	auto &slot = context.nameEntity(*n.id);
	if (slot.entity)
		context.logError("redefinition of entity");
	else
		slot.entity = data->type.translate(*n.type);
}


/*
f :: () {}
f2 :: () {}

g :: () -> (r : (u8)->()) {};

{
	// overloading symbols might be allowed?
	// what about function generators???
	// we would need to allow arbitrary overloading first and resolve everything during name resolution
	a :: f;
	a :: f2;
}

*/
void SemaStmtTranslator::visit(ast::ConstantDefinition &n) {
	auto &slot = context.nameEntity(*n.id);
	Entity *expr = data->expr.translate(*n.expr);
	if (slot.entity) {
		if (Constant::classof(slot.entity)) {
			// setup overload
			auto *c = (Constant*)slot.entity;
			if (OverloadedEntity::classof(c->expr))
				((OverloadedEntity*)c->expr)->entities.push_back(expr);
			else {
				auto *oe = context.create<OverloadedEntity>();
				oe->entities.push_back(expr);
				oe->entities.push_back(c->expr);
				c->expr = oe;
			}
		}
		else
			context.logError("redefinition of entity");
		return;
	}
	// setup new constant
	auto *c = context.create<Constant>(*n.id);
	slot.entity = c;
	c->expr = expr;
	if (n.type)
		c->type = data->type.translate(*n.type);
	else
		c->type = context.create<DependentType>(expr);
}

