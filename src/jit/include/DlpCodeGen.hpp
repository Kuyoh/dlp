#ifndef DLP_JIT_DLPCODEGEN_HPP
#define DLP_JIT_DLPCODEGEN_HPP

#include <string>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include "DlpAnalyzer.hpp"
#include "SemaEntityVisitor.hpp"

namespace dlp {
	constexpr const char *ENTRY_FUNC_NAME = "__entry";

	/*
	class DlpCodeGen {
	public:
		DlpCodeGen();
		~DlpCodeGen();
	
		void build(DlpAnalyzer::Program &ast, llvm::Module *module);
	
	protected:
		struct Data;
		Data *data;
	};
	*/

	struct CodeGenType : sema::SemaEntityVisitor {
		llvm::Type *result = nullptr;
		llvm::IRBuilder<> &builder;

		CodeGenType(llvm::IRBuilder<> &builder) : builder(builder) {}
		~CodeGenType() override = default;

		llvm::Type *translate(sema::Type *ast) {
			ast->visit(*this);
			return result;
		}

		void visitDefault() override;
		void visit(sema::TypeSymbol &n) override;
		void visit(sema::DependentType &n) override;
		void visit(sema::BoolType &n) override;
		void visit(sema::IntType &n) override;
		void visit(sema::FloatType &n) override;
		void visit(sema::StringType &n) override;
		void visit(sema::TypeType &n) override;
		void visit(sema::FunctionType &n) override;
		void visit(sema::PointerType &n) override;
		void visit(sema::StructType &n) override;
	};

	struct CodeGenFunc : sema::SemaEntityVisitor {
		llvm::IRBuilder<> &builder;
		llvm::ArrayRef<llvm::Value*> args;
		llvm::Value *result = nullptr;
		CodeGenType cgType;
		
		CodeGenFunc(llvm::IRBuilder<> &builder) : builder(builder), cgType(builder) {}

		llvm::Value *translateCall(sema::IFunction *f, llvm::ArrayRef<llvm::Value*> args);

		void visitDefault() override;
		void visit(sema::Function &n) override;
		void visit(sema::ImportedFunction &n) override;
	};

	struct CodeGenExpr : sema::SemaEntityVisitor {
		llvm::Value *result = nullptr;
		bool write = false;
		llvm::IRBuilder<> &builder;
		CodeGenType cgType;
		CodeGenFunc cgFunc;

		CodeGenExpr(llvm::IRBuilder<> &builder) : builder(builder), cgType(builder), cgFunc(builder) {}
		~CodeGenExpr() override = default;

		llvm::Value *translateRead(sema::Entity &n);
		llvm::Value *translateWrite(sema::Entity &n);

		void visitDefault() override;

		void visit(sema::Constant &n) override;
		void visit(sema::LiteralInt &n) override;
		void visit(sema::LiteralFloat &n) override;
		void visit(sema::LiteralString &n) override;
		void visit(sema::CallExpr &n) override;

		void visit(sema::Symbol &n) override;
		void visit(sema::MemAccessExpr &n) override;
	};
}
#endif
