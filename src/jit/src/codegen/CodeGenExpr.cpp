#include <sstream>
#include <iostream>
#include <unordered_map>
#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Verifier.h>
//#include "TypeSystem.hpp"
#include "DlpCodeGen.hpp"

// look at https://github.com/ldc-developers/ldc for reference
using namespace dlp;
using namespace dlp::sema;

llvm::Value *CodeGenExpr::translateRead(sema::Entity &n) {
	write = false;
	n.visit(*this);
	return result;
}

llvm::Value *CodeGenExpr::translateWrite(sema::Entity &n) {
	write = true;
	n.visit(*this);
	return result;
}

void CodeGenExpr::visitDefault() { result = nullptr; }

void CodeGenExpr::visit(Constant &n) {
	n.expr->visit(*this);
}
void CodeGenExpr::visit(LiteralInt &n) {
	if (n.llvmValue != nullptr) {
		result = n.llvmValue;
		return;
	}

	auto *llvmType = cgType.translate(n.type);
	n.llvmValue = result = llvm::ConstantInt::get(llvmType, n.value, llvm::cast<IntType>(n.type)->isSigned);
}
void CodeGenExpr::visit(LiteralFloat &n) {
	if (n.llvmValue != nullptr) {
		result = n.llvmValue;
		return;
	}

	n.llvmValue = result = llvm::ConstantFP::get(cgType.translate(n.type), n.value);
	// TODO: handle special values....
//static Constant * ConstantFP::getNaN (Type *Ty, bool Negative=false, unsigned type=0)
//static Constant * ConstantFP::getNegativeZero (Type *Ty)
//static Constant * ConstantFP::getInfinity (Type *Ty, bool Negative=false)
}
void CodeGenExpr::visit(LiteralString &n) {
	if (n.llvmValue != nullptr) {
		result = n.llvmValue;
		return;
	}

	n.llvmValue = result = builder.CreateGlobalStringPtr(n.value);
}

void CodeGenExpr::visit(CallExpr &n) {
	std::vector<llvm::Value*> args;
	args.reserve(n.arguments.size());
	for (auto a : n.arguments)
		args.push_back(translateRead(*a));
	
	// TODO: resolve func n.func
	// TODO: handle intrinsics (especially operators)
	// TODO: handle function objects
	result = cgFunc.translateCall(n.func, args);
	//llvm::Function *func = cgFunc.translate(n.func);
	//llvm::Function *func = resolveFunc(n.func);
	//result = builder.CreateCall(func, args);
	//builder.CreateCall()
}

void CodeGenExpr::visit(Symbol &n) {
	if (write) // resolve to address
		result = n.llvmAddress; // llvmAddress needed to be set at function level!
	else // resolve to value
		result = builder.CreateLoad(n.llvmAddress);
}
void CodeGenExpr::visit(MemAccessExpr &n) {
	llvm::Value *object = translateRead(*n.object);
	auto *st = llvm::dyn_cast<StructType>(n.object->type);
	llvm::Value *addr = nullptr;
	if (st != nullptr) {
		auto it = std::find_if(st->attributes.begin(), st->attributes.end(), [&](auto &a){ return a.name == n.memName; });
		if (it != st->attributes.end()) {
			size_t idx = it - st->attributes.begin();
	
			addr = builder.CreateStructGEP(st->llvmType, addr, idx);
			//std::vector<llvm::Value*> indices = { builder.getInt32(0), builder.getInt32(idx) };
			//std::vector<Value*> indices = { builder.getInt64(0), builder.getInt64(idx) };
			//addr = builder.CreateGEP(addr, indices);
		}
	}
	// TODO: else check builtin

	if (write) // resolve to address
		result = addr;
	else // resolve to value
		result = builder.CreateLoad(addr);
}

// void visit(TypeType &n) { visitDefault(); }
// void visit(BoolType &n) { visitDefault(); }
// void visit(IntType &n) { visitDefault(); }
// void visit(FloatType &n) { visitDefault(); }
// void visit(StringType &n) { visitDefault(); }
// void visit(FunctionType &n) { visitDefault(); }
// void visit(PointerType &n) { visitDefault(); }
// void visit(StructType &n) { visitDefault(); }
// void visit(TypeSymbol &n) { visitDefault(); }
// void visit(DependentType &n) { visitDefault(); }

/*
struct CodeGenVisitor : public ast::Visitor {
	llvm::LLVMContext &context;
	llvm::IRBuilder<> builder;
	llvm::IRBuilderBase::InsertPoint globalInitIp;
	TypeVisitor typeVisitor;
	llvm::Module *module;
	std::shared_ptr<Entity> result;
	llvm::Function *entryFunc;
	llvm::Function *findDynSymbolFunc;
	Scope rootScope;
	Scope *currentScope = &rootScope;

	//static constexpr const char *THIS_NAME = "self";
	CodeGenVisitor(llvm::Module *module) :
		context(module->getContext()), builder(context), typeVisitor(builder, currentScope), module(module)
	{
		std::vector<llvm::Type*> findDynSymbolArgTypes;
		findDynSymbolArgTypes.push_back(llvm::Type::getInt8PtrTy(context)); //char*
		findDynSymbolArgTypes.push_back(llvm::Type::getInt8PtrTy(context)); //char*

		llvm::FunctionType* findDynSymbolType = llvm::FunctionType::get(
			llvm::Type::getInt8PtrTy(context)->getPointerTo(), findDynSymbolArgTypes, false);

		findDynSymbolFunc = llvm::Function::Create(
			findDynSymbolType, llvm::Function::ExternalLinkage, "__findDynSymbol", module);

		auto *entryFuncType = llvm::FunctionType::get(builder.getVoidTy(), false);
		entryFunc = llvm::Function::Create(entryFuncType, llvm::GlobalValue::InternalLinkage, ENTRY_FUNC_NAME, module);
		rootScope.block = llvm::BasicBlock::Create(context, "entry", entryFunc, 0);
		builder.SetInsertPoint(rootScope.block);
		globalInitIp = builder.saveIP();
	}

	void logError(const char *err) {
		std::cerr << err << std::endl;
	}

	void processScope(sema::Scope &s) {
		// TODO: figure out how to deal with block sub-scopes
		
	//struct EntitySlot { Entity *entity; };
	//std::list<Symbol*> variables;
	//std::unordered_map<std::string, EntitySlot> namedEntities;
	//std::list<std::unique_ptr<Statement>> statements;
	//std::list<std::unique_ptr<Entity>> entities;
	//std::list<std::unique_ptr<Scope>> children;
	//Scope *parent;

	// we replace all this with a code-gen translation step; or we make it possible to have llvm::Value members in entities... forward declaration?
		for (auto *s : s.variables) {
			auto *type = typeVisitor.translate(s->type); // get llvm type
			llvm::Value *var = entryBuilder.CreateAlloca(type, 0, s->name.c_str());
			// TODO: associate variable name/symbol with llvm::Value representing the variable adress
		}

	}

	void processFunction(sema::Scope &s, llvm::FunctionType *fType, const char *fName = nullptr) {
		auto *func = llvm::Function::Create(fType, llvm::GlobalValue::InternalLinkage, fName != nullptr ? fName : "", module);
		auto *block = llvm::BasicBlock::Create(context, "entry", func, 0);

		auto ipSave = builder.saveIP();
		builder.SetInsertPoint(block);
		processScope(s);
		builder.restoreIP(ipSave);
	}
	
	void visitDefault() override { logError("unexpected node"); }

	inline llvm::Value *implicitCast(llvm::Value *value, llvm::Type *resultType) {
		auto *valType = value->getType();
		if (valType->getTypeID() == resultType->getTypeID()) return value;
		if (valType->isArrayTy() && resultType->isPointerTy()) {
			if (llvm::ConstantDataArray *constVal = llvm::dyn_cast_or_null<llvm::ConstantDataArray>(value)) {
				auto *GV = new llvm::GlobalVariable(
					*module, valType, true, llvm::GlobalVariable::PrivateLinkage, constVal);

				llvm::Value *idx = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0, false);
				llvm::Value *indexList[2] = { idx, idx };
				return builder.CreateGEP(
					GV->getType()->getPointerElementType(), GV,
					llvm::ArrayRef<llvm::Value*>(indexList, 2));
			}
		}
		switch (resultType->getTypeID()) {
		case llvm::Type::DoubleTyID:
			if (valType->isIntegerTy())
				return builder.CreateCast(llvm::Instruction::CastOps::SIToFP, value, resultType);
			break;
		default: break;
		}
		return value;
	}

	std::shared_ptr<Value> resolveSymbol(std::string &name) {
		auto sym = currentScope->findName(name);
		if (sym && llvm::isa<Value>(sym.get()))
			return std::static_pointer_cast<Value>(sym);
		return nullptr;
	}

	// Expressions
	void visit(ast::StringLiteral &n) override {
		std::string v = processStringLiteral(*n.value);
		result.reset(new Value(builder.CreateGlobalStringPtr(v, "", 0), // TODO: clean up pointer type creation here!
			std::make_shared<PointerType>(typeVisitor.builtinTypes["u8"]->getType()->getPointerTo())));
	}
	void visit(ast::IntegerLiteral &n) override {
		result.reset(new Constant(llvm::ConstantInt::get(builder.getInt32Ty(), n.value, true), typeVisitor.builtinTypes["s32"]));
	}
	void visit(ast::FloatLiteral &n) override {
		result.reset(new Constant(llvm::ConstantFP::get(builder.getDoubleTy(), n.value), typeVisitor.builtinTypes["f64"]));
	}

	void visit(ast::Identifier &n) override {
		result = resolveSymbol(*n.name);
		if (!result) {
			std::string err = "undeclared variable '" + *n.name + "'";
			logError(err.c_str());
		}
	}
	void visit(ast::MethodCall &n) override {
		result = nullptr;
		auto funcVal = resolveSymbol(*n.id);
		if (!funcVal) {
			std::string err = "unable to resolve function '" + *n.id + "'";
			logError(err.c_str());
			return;
		}

		llvm::FunctionType *funcType = nullptr;
		llvm::Value *val = funcVal->readValue(builder);
		if (auto *func = llvm::dyn_cast_or_null<llvm::Function>(val))
			funcType = func->getFunctionType();
		else if (auto *funcPtrType = llvm::dyn_cast_or_null<llvm::PointerType>(val->getType()))
			funcType = llvm::dyn_cast_or_null<llvm::FunctionType>(funcPtrType->getElementType());

		if (funcType != nullptr) {
			auto &argList = funcType->params();
			if (argList.size() >= n.arguments.size() || funcType->isVarArg()) {
				std::vector<llvm::Value*> args;
				auto it = argList.begin();
				for (auto &a : n.arguments) {
					a->visit(*this);
					auto *v = llvm::dyn_cast<Value>(result.get())->readValue(builder);
					if (it != argList.end()) {
						args.push_back(implicitCast(v, *it));
						++it;
					}
					else
						args.push_back(v);
				}
				auto *resultVal = builder.CreateCall(val, makeArrayRef(args), "");
				result.reset(new Value(resultVal, std::make_shared<BasicType>(resultVal->getType(), true))); // TODO: clean up basic type creation
			}
			else {
				std::string err = "not enough arguments calling function '" + *n.id + "'";
				logError(err.c_str());
			}
		}
		else {
			std::string err = "undeclared function '" + *n.id + "'";
			logError(err.c_str());
		}
	}
	void visit(ast::UnaryOperator &n) override {
		n.operand->visit(*this); auto operand = result; result = nullptr;
		if (!operand) return;
		switch (n.op) {
		case UnaryOperatorType::MINUS: {
			auto *resultVal = builder.CreateNeg(llvm::dyn_cast<Value>(operand.get())->readValue(builder));
			result.reset(new Value(resultVal, std::make_shared<BasicType>(resultVal->getType(), true))); return;
		}
		default:
			logError("invalid binary operator"); return;
		}
	}
	void visit(ast::BinaryOperator &n) override {
		n.lhs->visit(*this);
		if (!result) return;
		auto *l = llvm::dyn_cast<Value>(result.get())->readValue(builder);

		n.rhs->visit(*this);
		if (!result) return;
		auto *r = llvm::dyn_cast<Value>(result.get())->readValue(builder);
		result = nullptr;

		// promote types
		bool useFloatOp = false;
		if (l->getType()->isDoubleTy()) {
			r = implicitCast(r, l->getType());
			useFloatOp = true;
		}
		else if (r->getType()->isDoubleTy()) {
			l = implicitCast(l, r->getType());
			useFloatOp = true;
		}

		// in general, consider: https://llvm.org/docs/LangRef.html#intrinsic-global-variables
		// and: https://llvm.org/docs/LangRef.html#intrinsics

		if (useFloatOp) {
			switch (n.op) {
			case BinaryOperatorType::COMP_EQ:	{ auto *resVal = builder.CreateFCmpUEQ(l, r);	result.reset(new Value(resVal, std::make_shared<BasicType>(resVal->getType(), true))); } return; // u means we allow nan
			case BinaryOperatorType::COMP_NEQ:	{ auto *resVal = builder.CreateFCmpUNE(l, r);	result.reset(new Value(resVal, std::make_shared<BasicType>(resVal->getType(), true))); } return;
			case BinaryOperatorType::COMP_LT:	{ auto *resVal = builder.CreateFCmpULT(l, r);	result.reset(new Value(resVal, std::make_shared<BasicType>(resVal->getType(), true))); } return;
			case BinaryOperatorType::COMP_LE:	{ auto *resVal = builder.CreateFCmpULE(l, r);	result.reset(new Value(resVal, std::make_shared<BasicType>(resVal->getType(), true))); } return;
			case BinaryOperatorType::COMP_GT:	{ auto *resVal = builder.CreateFCmpUGT(l, r);	result.reset(new Value(resVal, std::make_shared<BasicType>(resVal->getType(), true))); } return;
			case BinaryOperatorType::COMP_GE:	{ auto *resVal = builder.CreateFCmpUGE(l, r);	result.reset(new Value(resVal, std::make_shared<BasicType>(resVal->getType(), true))); } return;
			case BinaryOperatorType::PLUS:  	{ auto *resVal = builder.CreateFAdd(l, r);		result.reset(new Value(resVal, std::make_shared<BasicType>(resVal->getType(), true))); } return;
			case BinaryOperatorType::MINUS: 	{ auto *resVal = builder.CreateFSub(l, r);		result.reset(new Value(resVal, std::make_shared<BasicType>(resVal->getType(), true))); } return;
			case BinaryOperatorType::MUL:   	{ auto *resVal = builder.CreateFMul(l, r);		result.reset(new Value(resVal, std::make_shared<BasicType>(resVal->getType(), true))); } return;
			case BinaryOperatorType::DIV:   	{ auto *resVal = builder.CreateFDiv(l, r);		result.reset(new Value(resVal, std::make_shared<BasicType>(resVal->getType(), true))); } return;
			default: logError("invalid binary operator"); return;
			}
		}
		else {
			switch (n.op) {
			case BinaryOperatorType::COMP_EQ:	{ auto *resVal = builder.CreateICmpEQ(l, r);	result.reset(new Value(resVal, std::make_shared<BasicType>(resVal->getType(), true))); } return;
			case BinaryOperatorType::COMP_NEQ:	{ auto *resVal = builder.CreateICmpNE(l, r);	result.reset(new Value(resVal, std::make_shared<BasicType>(resVal->getType(), true))); } return;
			case BinaryOperatorType::COMP_LT:	{ auto *resVal = builder.CreateICmpSLT(l, r);	result.reset(new Value(resVal, std::make_shared<BasicType>(resVal->getType(), true))); } return;
			case BinaryOperatorType::COMP_LE:	{ auto *resVal = builder.CreateICmpSLE(l, r);	result.reset(new Value(resVal, std::make_shared<BasicType>(resVal->getType(), true))); } return;
			case BinaryOperatorType::COMP_GT:	{ auto *resVal = builder.CreateICmpSGT(l, r);	result.reset(new Value(resVal, std::make_shared<BasicType>(resVal->getType(), true))); } return;
			case BinaryOperatorType::COMP_GE:	{ auto *resVal = builder.CreateICmpSGE(l, r);	result.reset(new Value(resVal, std::make_shared<BasicType>(resVal->getType(), true))); } return;
			case BinaryOperatorType::PLUS:		{ auto *resVal = builder.CreateAdd(l, r);		result.reset(new Value(resVal, std::make_shared<BasicType>(resVal->getType(), true))); } return;
			case BinaryOperatorType::MINUS:		{ auto *resVal = builder.CreateSub(l, r);		result.reset(new Value(resVal, std::make_shared<BasicType>(resVal->getType(), true))); } return;
			case BinaryOperatorType::MUL:		{ auto *resVal = builder.CreateMul(l, r);		result.reset(new Value(resVal, std::make_shared<BasicType>(resVal->getType(), true))); } return;
			case BinaryOperatorType::DIV:		{ auto *resVal = builder.CreateSDiv(l, r);		result.reset(new Value(resVal, std::make_shared<BasicType>(resVal->getType(), true))); } return;
			default: logError("invalid binary operator"); return;
			}
		}
	}

	struct BlockVisitor : Visitor {
		std::function<void(ast::Block&)> bbv;
		std::function<void(ast::ExternBlock&)> ebv;
		template <typename A, typename B> BlockVisitor(A &&a, B &&b) : bbv(a), ebv(b) {}
		virtual void visit(ast::Block &n) { bbv(n); }
		virtual void visit(ast::ExternBlock &n) { ebv(n); }
	};

	// for (iter) stmt/block
	// defer stmt/block

	// switch (expr) {
	// default: fallthrough;
	// case log(x) >  10;
	//		printf("test");
	// }

	// yield syntax like return...

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// next steps: basic types,
	// switch, defer, yield, types in declarations, nested blocks, lambdas
	// types, optionals, refactoring...

	void visit(ast::DefinitionExpression &n) override {
		n.type->visit(typeVisitor);
		auto defType = typeVisitor.result; typeVisitor.result.reset();
		if (auto *fType = llvm::dyn_cast_or_null<FunctionType>(defType.get())) {
			ast::FunctionType *astFType = static_cast<ast::FunctionType*>(n.type.get());
			BlockVisitor bv{ [=](ast::Block &n) {
				auto *function = llvm::Function::Create(fType->getType(), llvm::GlobalValue::InternalLinkage, "", module);
				auto *bblock = llvm::BasicBlock::Create(context, "entry", function, 0);
				currentScope = currentScope->addScope(bblock);
				builder.SetInsertPoint(bblock);

				auto argsValues = function->arg_begin();
				for (auto &v : astFType->arguments) {
					auto *argumentValue = &*argsValues++;
					argumentValue->setName(v->id->c_str());
					currentScope->namedValues.insert({ *v->id, std::make_shared<Value>(argumentValue, std::make_shared<BasicType>(argumentValue->getType(), true)) });
				}

				for (auto &s : n.statements)
					s->visit(*this);
				builder.CreateRetVoid(); // temporary fallback if return statement missing

				std::string errStr;
				llvm::raw_string_ostream ss(errStr);
				if (llvm::verifyFunction(*function, &ss)) {
					ss.flush();
					logError(errStr.c_str());
				}

				currentScope = currentScope->parent;
				builder.SetInsertPoint(currentScope->getBlock());
				result.reset(new Value(function, std::make_shared<BasicType>(function->getType(), false)));
			}, [=](ast::ExternBlock &n) {
				result = nullptr;

				n.lib->visit(*this);
				if (!result) return;
				auto *libVal = llvm::dyn_cast<Value>(result.get())->readValue(builder);

				n.func->visit(*this);
				if (!result) return;
				auto *funcVal = llvm::dyn_cast<Value>(result.get())->readValue(builder);
				result = nullptr;

				std::vector<llvm::Value*> args = {
					implicitCast(libVal, llvm::Type::getInt8PtrTy(context)),
					implicitCast(funcVal, llvm::Type::getInt8PtrTy(context)),
				};
				llvm::Value *faddr = builder.CreateCall(findDynSymbolFunc, makeArrayRef(args), "");

				auto *resVal = builder.CreateBitOrPointerCast(faddr, fType->getType()->getPointerTo());
				result.reset(new Value(resVal, std::make_shared<BasicType>(resVal->getType(), false)));
			} };
			n.block->visit(bv);
		}
		else {
			BlockVisitor bv{ [=](ast::Block &n) {
				// 1. create struct type stub
				// 2. set up constructor func
				// 3. parse constructer and define struct at the same  time (add local variables to struct)
				// what do we do about knowledge of the size of the struct?
				std::shared_ptr<StructType> strType(StructType::createStub(context));
				//void 	setBody(ArrayRef< Type *> Elements, bool isPacked = false)
				//	Specify a body for an opaque identified type.More...
				// TODO: what about struct name resolution??
				// we need to delay struct type generation until it is bound to a name or argument

				// TODO: this does not work yet, we probably require the struct to be defined before creating the constructor!
				// Thus we need type inference without code generation!

				// => maybe we want to passes
				// one for building scopes & inferring types, one for generating IR

				//struct DefVisitor : ast::Visitor {
				//	StructType *strType;
				//	virtual void visit(ast::VariableDefinition &n) { strType->addAttributeName(*n.id); }
				//	virtual void visit(ast::Alias &) { }
				//	virtual void visit(ast::ConstantDefinition &) {  }
				//} defVis;
				//defVis.strType = strType.get();
				//for (auto &s : n.statements)
				//	s->visit(defVis);

				auto *fType = llvm::FunctionType::get(builder.getVoidTy(), { strType->getType()->getPointerTo() }, false);
				auto *function = llvm::Function::Create(fType, llvm::GlobalValue::InternalLinkage, "", module);
				auto *bblock = llvm::BasicBlock::Create(context, "entry", function, 0);
				currentScope = currentScope->addScope(bblock);
				builder.SetInsertPoint(bblock);
				//currentScope->definingType = strType.get();

				auto argsValues = function->arg_begin();
				auto *thisValue = &*argsValues;
				//thisValue->setName(THIS_NAME);
				//auto self = std::make_shared<Value>(thisValue);
				//currentScope->namedValues.insert({ THIS_NAME, self });
				//currentScope->self = self.get();

				for (auto &s : n.statements)
					s->visit(*this);

				// now we can finish the struct
				for (auto &p : currentScope->namedValues) {
					if (auto *val = llvm::dyn_cast<Value>(p.second.get()))
						strType->setAttribute({p.first(), val->getType()});
				}
				strType->finalizeStruct();

				// set all values
				for (auto &p : currentScope->namedValues) {
					auto *ePtr = strType->accessAttribute(builder, p.first(), thisValue);
					if (auto *val = llvm::dyn_cast<Value>(p.second.get()))
						builder.CreateStore(val->readValue(builder), ePtr);
				}
				builder.CreateRetVoid(); // temporary fallback if return statement missing

				std::string errStr;
				llvm::raw_string_ostream ss(errStr);
				if (llvm::verifyFunction(*function, &ss)) {
					ss.flush();
					logError(errStr.c_str());
				}

				currentScope = currentScope->parent;
				builder.SetInsertPoint(currentScope->getBlock());

				strType->setConstructor(function);
				result = strType;
			}, [=](ast::ExternBlock &n) {
				logError("external types are not supported");
			} };
			n.block->visit(bv);
		}
	}

	void visit(ast::Block &n) override {
		currentScope = currentScope->addScope(nullptr);
		for (auto &s : n.statements)
			s->visit(*this);
		currentScope = currentScope->parent;
	}
	void visit(ast::ExternBlock &) override { visitDefault(); }

	// Statements
	void visit(ast::IfStatement &n) override {
		n.expression->visit(*this);
		if (!result) return;
		auto *exprVal = llvm::dyn_cast<Value>(result.get())->readValue(builder);
		result = nullptr;
		
		auto *func = builder.GetInsertBlock()->getParent();
		llvm::BasicBlock *thenBlock = llvm::BasicBlock::Create(context, "then", func);
		llvm::BasicBlock *elseBlock = llvm::BasicBlock::Create(context, "else");
		llvm::BasicBlock *mergeBlock = llvm::BasicBlock::Create(context, "ifcont");
		
		builder.CreateCondBr(exprVal, thenBlock, elseBlock);
		
		// then block
		builder.SetInsertPoint(thenBlock);
		n.thenBlock->visit(*this);
		
		builder.CreateBr(mergeBlock);
		
		// else block
		func->getBasicBlockList().push_back(elseBlock);
		builder.SetInsertPoint(elseBlock);
		if (n.elseBlock) {
			n.elseBlock->visit(*this);
		}
		builder.CreateBr(mergeBlock);
		
		// merge block
		func->getBasicBlockList().push_back(mergeBlock);
		builder.SetInsertPoint(mergeBlock);
	}

	void visit(ast::ForStatement &n) override {
		auto *func = builder.GetInsertBlock()->getParent();
		auto *preLoopBlock = builder.GetInsertBlock();
		
		llvm::BasicBlock *loopConditionBlock = llvm::BasicBlock::Create(context, "beforeloop", func);
		llvm::BasicBlock *loopBlock = llvm::BasicBlock::Create(context, "loop");
		llvm::BasicBlock *mergeBlock = llvm::BasicBlock::Create(context, "forcont");
		builder.CreateBr(loopConditionBlock); // jump from preLoopBlock to loopBlock
		
		// define loopConditionBlock
		auto ipSave = builder.saveIP();
		builder.SetInsertPoint(loopConditionBlock); // jump to loop block

		n.expression->visit(*this);
		if (!result) {
			builder.restoreIP(ipSave);
			return;
		}
		auto *exprVal = llvm::dyn_cast<Value>(result.get())->readValue(builder);
		builder.CreateCondBr(exprVal, loopBlock, mergeBlock);
		
		// define loopBlock
		func->getBasicBlockList().push_back(loopBlock);
		builder.SetInsertPoint(loopBlock); // jump to loop block
		n.block->visit(*this);
		builder.CreateBr(loopConditionBlock);
		
		// define afterLoopBlock
		func->getBasicBlockList().push_back(mergeBlock);
		builder.SetInsertPoint(mergeBlock); // jump to loop block
	}

	void visit(ast::Assignment &n) override {
		n.rhs->visit(*this);
		if (!result) return;
		auto var = resolveSymbol(*n.lhs);
		if (!var) {
			std::string err = "undeclared variable '" + *n.lhs + "'";
			logError(err.c_str());
		}
		else
			var->writeValue(builder, llvm::dyn_cast<Value>(result.get())->readValue(builder)); // TODO: check return value?
		result = nullptr;
	}
	void visit(ast::ExpressionStatement &n) override {
		n.expression->visit(*this);
		result = nullptr;
	}
	void visit(ast::ReturnStatement &n) override {
		if (n.expression) {
			n.expression->visit(*this);
			if (!result) return;
			builder.CreateRet(llvm::dyn_cast<Value>(result.get())->readValue(builder));
		}
		else builder.CreateRetVoid();
		result = nullptr;
	}
	void visit(ast::Alias &) override { logError("types not yet supported"); }
	void visit(ast::VariableDefinition &n) override {
		if (n.assignmentExpr != nullptr) {
			n.assignmentExpr->visit(*this);
			Value *exprResult = llvm::dyn_cast_or_null<Value>(result.get());
			if (exprResult == nullptr) return;
			//if (currentScope->definingType != nullptr) {
			//	currentScope->definingType->setAttribute({ *n.id, exprResult->getType() });
			//	llvm::Value *var = currentScope->definingType->accessAttribute(builder, *n.id, currentScope->self->readValue(builder));
			//	currentScope->namedValues.insert({ *n.id, std::make_shared<Variable>(var, exprResult->getType()) });
			//}
			//else {
				llvm::Value *val = exprResult->readValue(builder);
				llvm::Function *currentFunc = builder.GetInsertBlock()->getParent();
				llvm::IRBuilder<> entryBuilder(&currentFunc->getEntryBlock(), currentFunc->getEntryBlock().begin());
				llvm::Value *var = entryBuilder.CreateAlloca(val->getType(), 0, n.id->c_str());
				currentScope->namedValues.insert({ *n.id, std::make_shared<Variable>(var, exprResult->getType()) });
				builder.CreateStore(val, var);
			//}
		}
		else {
			n.type->visit(typeVisitor);
			auto type = typeVisitor.result; typeVisitor.result.reset();

			llvm::Function *currentFunc = builder.GetInsertBlock()->getParent();
			llvm::IRBuilder<> entryBuilder(&currentFunc->getEntryBlock(), currentFunc->getEntryBlock().begin());
			llvm::Value *var = entryBuilder.CreateAlloca(type->getType(), 0, n.id->c_str());
			currentScope->namedValues.insert({ *n.id, std::make_shared<Variable>(var, type) });
		}
		result = nullptr;
	}
	void visit(ast::ConstantDefinition &n) override {
		auto ip = builder.saveIP();
		builder.restoreIP(globalInitIp);
		n.expr->visit(*this);

		if (auto *cv = llvm::dyn_cast_or_null<Constant>(result.get())) {
			auto *gv = new llvm::GlobalVariable(
				*module, cv->constant->getType(), true, llvm::GlobalVariable::InternalLinkage, cv->constant);
			currentScope->namedValues.insert({ *n.id, std::make_shared<Variable>(gv, cv->getType()) });
		}
		else if (auto *v = llvm::dyn_cast_or_null<Value>(result.get())) {
			llvm::Value *val = v->readValue(builder);
			auto *gv = new llvm::GlobalVariable(
				*module, val->getType(), false, llvm::GlobalVariable::InternalLinkage, llvm::Constant::getNullValue(val->getType()));

			builder.CreateStore(val, gv);
			currentScope->namedValues.insert({ *n.id, std::make_shared<Variable>(gv, v->getType()) });
		}
		else if (auto *t = llvm::dyn_cast_or_null<Type>(result.get())) {
			currentScope->namedValues.insert({ *n.id, std::static_pointer_cast<Type>(result) });
		}
		globalInitIp = builder.saveIP();
		builder.restoreIP(ip);
		result = nullptr;
	}
};

struct DlpCodeGen::Data {
	std::unique_ptr<CodeGenVisitor> codeGenVisitor;
};

DlpCodeGen::DlpCodeGen() : data(new Data) {}
DlpCodeGen::~DlpCodeGen() { delete data; }


void DlpCodeGen::build(DlpAnalyzer::Program &ast, llvm::Module *module) {
	// TODO: modify everything for sema ast
	data->codeGenVisitor.reset(new CodeGenVisitor(module));


	data->codeGenVisitor->processFunction(*ast.program, ENTRY_FUNC_NAME, llvm::FunctionType::get(builder.getVoidTy(), false));
	//struct EntitySlot { Entity *entity; };
	//std::list<Symbol*> variables;
	//std::unordered_map<std::string, EntitySlot> namedEntities;
	//std::list<std::unique_ptr<Statement>> statements;
	//std::list<std::unique_ptr<Entity>> entities;
	//std::list<std::unique_ptr<Scope>> children;
	//Scope *parent;

	//for (auto &s : ast.program->statements)
	//	s->visit(*data->codeGenVisitor);
	//data->codeGenVisitor->builder.CreateRetVoid();

	// TODO: first clean up, encapsulate some of the data
	// better error handling - we also want to verify as much as possible

	std::string errStr;
	llvm::raw_string_ostream ss(errStr);
	if (llvm::verifyFunction(*data->codeGenVisitor->entryFunc, &ss)) {
		ss.flush();
		data->codeGenVisitor->logError(errStr.c_str());
	}
	else if (llvm::verifyModule(*module, &ss)) {
		ss.flush();
		data->codeGenVisitor->logError(errStr.c_str());
	}

	//auto *t = bblock->getTerminator();

	//bblock->getTerminator()
}
/**/

