#include "SemaTypeTranslator.hpp"
#include "SemaExprTranslator.hpp"

using namespace dlp;
using namespace dlp::sema;

struct SemaTypeTranslator::Data {
    SemaExprTranslator &expr;
    Type *result = nullptr;
    Data(SemaExprTranslator &expr) : expr(expr) {}
};

SemaTypeTranslator::SemaTypeTranslator(SemaTranslationContext &context, SemaExprTranslator &expr) :
	context(context), data(std::make_unique<Data>(expr)) {
}

SemaTypeTranslator::~SemaTypeTranslator() {}

Type *SemaTypeTranslator::translate(ast::IType &n) {
    n.visit(*this);
    return data->result;
}

void SemaTypeTranslator::visitDefault() {
#ifdef _MSC_VER    
    __debugbreak();
#endif
}
void SemaTypeTranslator::visit(ast::Identifier &n) {
    data->result = context.create<TypeSymbol>(*n.name);
}
void SemaTypeTranslator::visit(ast::PointerType &n) {
    data->result = context.create<PointerType>(translate(*n.type));
}
void SemaTypeTranslator::visit(ast::FunctionType &n) {
    std::vector<FunctionType::Argument> args;
    for (auto &a : n.arguments) {
        FunctionType::Argument arg;
        arg.name = *a->id;
		if (a->assignmentExpr)
			arg.defaultVal = data->expr.translate(*a->assignmentExpr);
		if (a->type)
			arg.type = translate(*a->type);
		else
            arg.type = context.create<DependentType>(arg.defaultVal);
        args.emplace_back(std::move(arg));
    }
    std::vector<FunctionType::Argument> outArgs;
    for (auto &a : n.arguments) {
        FunctionType::Argument arg;
        arg.name = *a->id;
		if (a->assignmentExpr)
			arg.defaultVal = data->expr.translate(*a->assignmentExpr);
		if (a->type)
			arg.type = translate(*a->type);
		else
            arg.type = context.create<DependentType>(arg.defaultVal);
        outArgs.emplace_back(std::move(arg));
    }
    data->result = context.create<FunctionType>(
        std::move(args), std::move(outArgs), n.arguments.isVarArg
    );
}
