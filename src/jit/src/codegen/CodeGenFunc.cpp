#include "DlpCodeGen.hpp"

using namespace dlp;
using namespace sema;

llvm::Value *CodeGenFunc::translateCall(IFunction *f, llvm::ArrayRef<llvm::Value*> args) {
    this->args = std::move(args);
    f->visit(*this);
    return result;
}

void CodeGenFunc::visitDefault() {
    result = nullptr;
}
void CodeGenFunc::visit(Function &n) {
    // TODO: what about captures??
    if (n.llvmFunction == nullptr) {
        // create function ...
        //auto *fType = (FunctionType*)(n.type);
        auto *llvmFType = cgType.translate(n.type);
        
        n.llvmFunction = llvm::Function::Create(
            llvm::cast<llvm::FunctionType>(llvmFType), llvm::Function::InternalLinkage,
            "", builder.GetInsertBlock()->getModule());
        
        
        //step 1: prepare func arguments
        //step 2: generate function code
        //n.scope
    }

    // TODO: add implicit cast for arguments (maybe in CodeGenExpr?)
    result = builder.CreateCall(n.llvmFunction, args);
    

    /*

			Function(FunctionType *type) : IFunction(Kind::FUNCTION, type) {}
			void visit(SemaEntityVisitor &v) override { v.visit(*this); }			
			static bool classof(const Entity *e) {
				return e->getKind() == Kind::FUNCTION;
			}
			Scope *scope;
		};
		struct ImportedFunction : IFunction {
			ImportedFunction(FunctionType *type) : IFunction(Kind::IMPORTEDFUNCTION, type) {}
			void visit(SemaEntityVisitor &v) override { v.visit(*this); }			
			static bool classof(const Entity *e) {
				return e->getKind() == Kind::IMPORTEDFUNCTION;
			}
			Entity *libName = nullptr;
            Entity *funcName = nullptr;
            */
}

void CodeGenFunc::visit(ImportedFunction &n) {

}
