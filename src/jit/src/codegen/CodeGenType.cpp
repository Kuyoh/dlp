#include <llvm/IR/IRBuilder.h>
#include "DlpCodeGen.hpp"

using namespace dlp;
using namespace dlp::sema;

void CodeGenType::visitDefault() {
    result = nullptr;
}

void CodeGenType::visit(TypeSymbol &n) {
    if (n.llvmType != nullptr) {
        result = n.llvmType;
        return;
    }
    n.llvmType = result = translate(n.resolvedType);
}

void CodeGenType::visit(DependentType &n) {
    if (n.llvmType != nullptr) {
        result = n.llvmType;
        return;
    }
    n.llvmType = result = translate(n.dependency->type);
}

void CodeGenType::visit(BoolType &n) {
    if (n.llvmType != nullptr) {
        result = n.llvmType;
        return;
    }

    n.llvmType = result = builder.getInt1Ty();
    //builder.getTrue()
}

void CodeGenType::visit(IntType &n) {
    if (n.llvmType != nullptr) {
        result = n.llvmType;
        return;
    }

    n.llvmType = result = builder.getIntNTy((int)n.bitness);
}

void CodeGenType::visit(FloatType &n) {
    if (n.llvmType != nullptr) {
        result = n.llvmType;
        return;
    }

    if (n.bitness == 64)
        n.llvmType = result = builder.getDoubleTy();
    else if (n.bitness == 32)
        n.llvmType = result = builder.getFloatTy();
    else // if (n.bitness == 16)
        n.llvmType = result = builder.getHalfTy();
}

void CodeGenType::visit(StringType &n) {
    if (n.llvmType != nullptr) {
        result = n.llvmType;
        return;
    }

    n.llvmType = result = builder.getInt8Ty()->getPointerTo();

// TODO: define a better string type...
// we want:
//  * reasonable small string optimization
//  * easy iterator range access
//  * reference counting
//  * immutability
//  * conversion to c-style string
//  * capability of accessing constant strings
//  * string view
}

void CodeGenType::visit(TypeType &n) {
    if (n.llvmType != nullptr) {
        result = n.llvmType;
        return;
    }

    // TODO: create type-structs
    // maybe we can make it llvm constants?
}

void CodeGenType::visit(FunctionType &n) {
    if (n.llvmType != nullptr) {
        result = n.llvmType;
        return;
    }

    std::vector<llvm::Type*> retTypes(n.results.size());
    for (auto &a : n.results)
        retTypes.push_back(translate(a.type));

    std::vector<llvm::Type*> argTypes(n.arguments.size());
    for (auto &a : n.arguments)
    argTypes.push_back(translate(a.type));

    llvm::Type *retType;
    if (retTypes.size() == 1)
        retType = retTypes[0];
    else if (retTypes.size() == 0)
        retType = builder.getVoidTy();
    else // if (retTypes.size() > 1)
        retType = llvm::StructType::create(builder.getContext(), retTypes);

    n.llvmType = result = llvm::FunctionType::get(retType, argTypes, n.isVarArg);
}

void CodeGenType::visit(PointerType &n) {
    if (n.llvmType != nullptr) {
        result = n.llvmType;
        return;
    }
    auto *type = translate(n.elementType);
    if (type != nullptr)
        n.llvmType = result = type->getPointerTo();
    else
        n.llvmType = result = nullptr;
}

void CodeGenType::visit(StructType &n) {
    if (n.llvmType != nullptr) {
        result = n.llvmType;
        return;
    }
    llvm::StructType *strType;
    n.llvmType = result = strType = llvm::StructType::create(builder.getContext());
    std::vector<llvm::Type*> attrTypes(n.attributes.size());
    for (auto &a : n.attributes) {
        auto *aType = translate(a.type);
        if (auto *structType = llvm::dyn_cast<StructType>(a.type)) {
            if (structType == &n) {
                //logError("recursive member type is not allowed");
                result = nullptr;
                return;
            }
        }
        attrTypes.push_back(aType);
    }
    strType->setBody(attrTypes);
}
