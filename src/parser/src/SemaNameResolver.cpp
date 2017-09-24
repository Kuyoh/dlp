#include "SemaNameResolver.hpp"
#include "SemaStatement.hpp"
#include "SemaOverloadSelection.hpp"
#include <algorithm>

using namespace dlp;
using namespace sema;

struct SemaNameResolver::Data {
	Scope *scope = nullptr;
	Type *resolveType(Type *type) {
		if (TypeSymbol::classof(type))
			return resolveType(((TypeSymbol*)type)->resolvedType);
		else if (DependentType::classof(type))
			return resolveType(((DependentType*)type)->dependency->type);
		else
			return type;
	}
	Entity *resolveEntity(Entity *entity) {
		if (Symbol::classof(entity))
			return ((Symbol*)entity)->entity;
		return entity;
	}
};

SemaNameResolver::SemaNameResolver(SemaTranslationContext &context) : context(context), data(new Data) {}
SemaNameResolver::~SemaNameResolver() {}

void SemaNameResolver::visit(Scope *s) {
	Scope *tmp = data->scope;
	data->scope = s;
	for (auto &s : s->statements)
		s->visit(*this);
	data->scope = tmp;
}

void SemaNameResolver::visit(BlockStatement &n) {
	visit(n.scope);
}

void SemaNameResolver::visit(ConditionalStatement &n) {
	n.expr->visit(*this);
	visit(n.thenBlock);
	if (n.elseBlock != nullptr)
		visit(n.elseBlock);
}

void SemaNameResolver::visit(LoopStatement &n) {
	n.expr->visit(*this);
	visit(n.loopBlock);
}

void SemaNameResolver::visit(ReturnStatement &n) {
	n.expr->visit(*this);
}

void SemaNameResolver::visit(AssignmentStatement &n) {
	n.lhs->visit(*this);
	n.rhs->visit(*this);
}

void SemaNameResolver::visit(ExpressionStatement &n) {
	n.expr->visit(*this);
}

void SemaNameResolver::visit(FunctionType &n) {
	for (auto &a : n.arguments) {
		a.type->visit(*this);
		a.type = data->resolveType(a.type);
		if (a.defaultVal != nullptr)
			a.defaultVal->visit(*this);
	}
	for (auto &a : n.results) {
		a.type->visit(*this);
		a.type = data->resolveType(a.type);
		if (a.defaultVal != nullptr)
			a.defaultVal->visit(*this);
	}
}

void SemaNameResolver::visit(PointerType &n) {
	n.elementType->visit(*this);
	n.elementType = data->resolveType(n.elementType);
}

void SemaNameResolver::visit(StructType &n) {
	if (n.attributes.size() == 0) {
		for (auto *v : n.scope->variables) {
			v->type->visit(*this);
			n.attributes.push_back({ v->name, data->resolveType(v->type) });
		}
	}
	visit(n.scope);
}

void SemaNameResolver::visit(TypeSymbol &n) {
	if (n.resolvedType != nullptr) {
		auto *entity = context.resolveEntity(n.name);
		if (entity != nullptr) {
			entity->visit(*this);
			entity = data->resolveEntity(entity);
			if (Type::classof(entity))
				n.resolvedType = (Type*)(entity);
			else
				context.logError("symbol is does not name a type");
		}
		else
			context.logError("unresolved type");
	}
}

void SemaNameResolver::visit(DependentType &n) {
	n.dependency->visit(*this);
}

void SemaNameResolver::visit(Function &n) {
	// TODO: ADD arguments to scope ?
	visit(n.scope);
}

void SemaNameResolver::visit(OverloadedEntity &n) {
	if (n.overloads.size() > 0)
		return;
	for (auto &a : n.entities) {
		a->visit(*this);
		// TODO: entity should be computed as it should be const!
		// but we can only do it with a code generator
		auto *e = data->resolveEntity(a);
		if (!FunctionType::classof(e->type)) {
			context.logError("only function types may be overloaded");
			return;
		}
		n.overloads.push_back((FunctionType*)e->type);
	}
}

void SemaNameResolver::visit(Symbol &n) {
	if (n.entity != nullptr)
		return;
	auto *entity = context.resolveEntity(n.name);
	if (entity == nullptr) {
		context.logError("unresolved symbol");
		return;
	}
	n.entity = entity;
	entity->visit(*this);
	n.type = entity->type;
}

void SemaNameResolver::visit(CallExpr &n) {
	if (n.funcEntity != nullptr)
		return;

	std::vector<Type*> argTypes;
	for (auto *a : n.arguments) {
		a->visit(*this);
		argTypes.emplace_back(a->type);
	}
	auto *entity = context.resolveEntity(n.funcName);
	if (entity != nullptr) {
		entity->visit(*this);
		FunctionType *fType = nullptr;
		if (OverloadedEntity::classof(entity)) {
			auto *oe = (OverloadedEntity*)entity;
			int idx = selectOverload(argTypes, oe->overloads);

			// TODO: implement overload resolution here
			//oe->overloads
			//fType = ??
		}
		else if (FunctionType::classof(entity->type)) {
			n.funcEntity = entity;
			fType = ((FunctionType*)entity->type);
			// TODO: assert arg type compatibility!
			// OR unify this with overloading...
		}
		else
			context.logError("Name does not refer to a function");
		if (fType != nullptr)
			n.type = fType->results[0].type;
		// TODO: implement tuple-types for multiple return values!
	}
	else
		context.logError("unresolved function");
}

void SemaNameResolver::visit(MemAccessExpr &n) {
	n.object->visit(*this);
	if (StructType::classof(n.object->type)) {
		auto structType = (StructType*)n.object->type;
		auto it = std::find_if(structType->attributes.begin(), structType->attributes.end(),
			[&](auto &a) { return a.name == n.memName; });
		if (it != structType->attributes.end()) {
			n.type = it->type;
		}
		else
			context.logError("Invalid name for member access");
	}
	else
		context.logError("Invalid object type for member access");
}
