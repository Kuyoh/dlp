#include "SemaExprTranslator.hpp"
#include "SemaStmtTranslator.hpp"
#include <sstream>
#include <algorithm>
#include <functional>

using namespace dlp;
using namespace dlp::sema;

struct SemaExprTranslator::Data {
    SemaStmtTranslator &stmt;
    Entity *result = nullptr;
    Data(SemaStmtTranslator &stmt) : stmt(stmt) {}
};

std::string processStringLiteral(std::string &in);

SemaExprTranslator::SemaExprTranslator(SemaTranslationContext &context, SemaStmtTranslator &stmt) :
	context(context), data(std::make_unique<Data>(stmt)) {

}
SemaExprTranslator::~SemaExprTranslator() {}

Entity *SemaExprTranslator::translate(ast::IExpression &n) {
    n.visit(*this);
    return data->result;
}

void SemaExprTranslator::visitDefault() {
#ifdef _MSC_VER
    __debugbreak();
#endif
}

// Check "CppCon 2015: Andrei Alexandrescu "std::allocator" for allocator info...
void SemaExprTranslator::visit(ast::StringLiteral &n) {
	Type *type;
	if (n.suffix)
		type = context.create<TypeSymbol>(*n.suffix);
	else
		type = context.create<TypeSymbol>("string");
	data->result = context.create<LiteralString>(processStringLiteral(*n.value), type);
}

// TODO: check whether the result value fits in the range of the corresponding type
// can only be done after name resolution!
void SemaExprTranslator::visit(ast::HexLiteral &n) {
	Type *type;
	if (n.suffix)
		type = context.create<TypeSymbol>(*n.suffix);
	else
		type = context.create<TypeSymbol>("u32");

	std::string v = n.value->substr(2);
	v.erase(std::remove(v.begin(), v.end(), '_'), v.end());

	uint64_t intVal;
	sscanf_s(v.c_str(), "%llx", &intVal);
	data->result = context.create<LiteralInt>(intVal, type);
}

void SemaExprTranslator::visit(ast::OctalLiteral &n) {
	Type *type;
	if (n.suffix)
		type = context.create<TypeSymbol>(*n.suffix);
	else
		type = context.create<TypeSymbol>("s32");

	std::string v = n.value->substr(1);
	v.erase(std::remove(v.begin(), v.end(), '_'), v.end());

	uint64_t intVal;
	sscanf_s(v.c_str(), "%llo", &intVal);
	data->result = context.create<LiteralInt>(intVal, type);
}

void SemaExprTranslator::visit(ast::DecimalLiteral &n) {
	Type *type;
	if (n.suffix)
		type = context.create<TypeSymbol>(*n.suffix);
	else
		type = context.create<TypeSymbol>("s32");

	std::string v = n.value->substr(1);
	v.erase(std::remove(v.begin(), v.end(), '_'), v.end());

	uint64_t intVal;
	sscanf_s(v.c_str(), "%llu", &intVal);
	data->result = context.create<LiteralInt>(intVal, type);
}

void SemaExprTranslator::visit(ast::FloatLiteral &n) {
	Type *type;
	if (n.suffix)
		type = context.create<TypeSymbol>(*n.suffix);
	else
		type = context.create<TypeSymbol>("f64");

	std::string v = n.value->substr(1);
	v.erase(std::remove(v.begin(), v.end(), '_'), v.end());

	double floatVal;
	sscanf_s(v.c_str(), "%lf", &floatVal);
    data->result = context.create<LiteralFloat>(floatVal, type);
}
void SemaExprTranslator::visit(ast::Identifier &n) {
    data->result = context.create<Symbol>(*n.name);
}

Entity *SemaExprTranslator::translateCall(std::string name, std::vector<Entity*> args) {
    auto *call = context.create<CallExpr>(name);
    std::swap(call->arguments, args);
    return call;
}

void SemaExprTranslator::visit(ast::MethodCall &n) {
    std::vector<Entity*> args;
    for (auto &a : n.arguments)
        args.push_back(translate(*a));
    data->result = translateCall(*n.id, args);
}
void SemaExprTranslator::visit(ast::UnaryOperator &n) {
    std::vector<Entity*> args = { translate(*n.operand) };
    std::string opName = "operator";
    opName += unOpToken2Str(n.op);
    data->result = translateCall(opName, args);
}
void SemaExprTranslator::visit(ast::BinaryOperator &n) {
    std::vector<Entity*> args = { translate(*n.lhs), translate(*n.rhs) };
    std::string opName = "operator";
    opName += binOpToken2Str(n.op);
    data->result = translateCall(opName, args);
}
void SemaExprTranslator::visit(ast::IndexAccess &n) {
    std::vector<Entity*> args;
    for (auto &a : n.arguments)
        args.push_back(translate(*a));
    data->result = translateCall("operator[]", args);
}
void SemaExprTranslator::visit(ast::MemberAccess &n) {
    auto *expr = context.create<MemAccessExpr>(*n.memberName);
    expr->object = translate(*n.aggregate);
    data->result = expr;
}

struct BlockVisitor : ast::Visitor {
	std::function<void(ast::Block&)> bbv;
	std::function<void(ast::ExternBlock&)> ebv;
	template <typename A, typename B> BlockVisitor(A &&a, B &&b) : bbv(a), ebv(b) {}
	virtual void visit(ast::Block &n) { bbv(n); }
	virtual void visit(ast::ExternBlock &n) { ebv(n); }
};
void SemaExprTranslator::visit(ast::DefinitionExpression &n) {
	Type *type = data->stmt.translateType(*n.type);
    if (FunctionType::classof(type)) { // function definition
        FunctionType *fType = static_cast<FunctionType*>(type);
		BlockVisitor bv{ [=](ast::Block &n) {
            auto *func = context.create<Function>(fType);

			func->scope = context.pushScope();
            for (auto &a : fType->arguments) {
                auto &es = context.nameEntity(a.name);
                es.entity = context.create<Argument>(a.name, a.type);
            }
            for (auto &a : fType->results) {
                auto &es = context.nameEntity(a.name);
                es.entity = context.create<Argument>(a.name, a.type);
            } // TODO: decide what to do about vararg
		    data->stmt.process(n.statements);
            context.popScope();

			data->result = func;
		}, [=](ast::ExternBlock &n) {
            auto *libName = translate(*n.lib);
            auto *funcName = translate(*n.func);
            auto *func = context.create<ImportedFunction>(fType);

            func->libName = libName;
            func->funcName = funcName;

            data->result = func;
		} };
		n.block->visit(bv);
    }
    else { // type definition
		BlockVisitor bv{ [=](ast::Block &n) {
            StructType *structType = context.create<StructType>();

			structType->scope = context.pushScope();
		    data->stmt.process(n.statements);
            context.popScope();

            data->result = structType;
		}, [=](ast::ExternBlock &n) {
            context.logError("extern types are not allowed");
            data->result = nullptr;
		} };
    }
}

std::string processStringLiteral(std::string &in) {
	std::stringstream ss;
	bool isEscaping = false;
	for (auto &c : in) {
		if (c == '\\' && !isEscaping) {
			isEscaping = true;
			continue;
		}
		if (isEscaping) {
			// TODO: add character value definition - see http://en.cppreference.com/w/cpp/language/escape
			switch (c) {
			case 'a': ss << '\a'; break;
			case 'b': ss << '\b'; break;
			case 'f': ss << '\f'; break;
			case 'n': ss << '\n'; break;
			case 'r': ss << '\r'; break;
			case 't': ss << '\t'; break;
			case 'v': ss << '\v'; break;
			case '0': ss << '\0'; break;
			default: ss << c; break;
			}
			isEscaping = false;
		}
		else
			ss << c;
	}
	return ss.str();
}

