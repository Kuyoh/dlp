#ifndef DLP_PARSER_SEMATYPES_HPP
#define DLP_PARSER_SEMATYPES_HPP

#include <string>
#include <memory>
#include "SemaEntity.hpp"

namespace dlp {
	namespace sema {
		struct Scope;
		struct Type : Entity {
			enum Bitness { B8 = 8, B16 = 16, B32 = 32, B64 = 64, B128 = 128 };

			static bool classof(const Entity *e) {
				return e->getKind() >= Kind::TYPETYPE && e->getKind() <= Kind::DEPENDENTTYPE;
			}
			llvm::Type *llvmType = nullptr;

		protected:
			Type(Kind kind) : Entity(kind) {
				type = typeType;
			}
			static Type *typeType;
		};
		struct TypeType : Type {
			TypeType() : Type(Kind::TYPETYPE) {}
			static bool classof(const Entity *e) {
				return e->getKind() == Kind::TYPETYPE;
			}
			void visit(SemaEntityVisitor &v) override { v.visit(*this); }
		};
		struct BoolType : Type {
			BoolType() : Type(Kind::BOOLTYPE) {}
			static bool classof(const Entity *e) {
				return e->getKind() == Kind::BOOLTYPE;
			}
			void visit(SemaEntityVisitor &v) override { v.visit(*this); }
		};
		struct IntType : Type {
			IntType(Bitness bitness, bool isSigned) : Type(Kind::INTTYPE), bitness(bitness), isSigned(isSigned) {}
			static bool classof(const Entity *e) {
				return e->getKind() == Kind::INTTYPE;
			}
			void visit(SemaEntityVisitor &v) override { v.visit(*this); }

			bool isSigned;
			Bitness bitness;
		};
		struct FloatType : Type {
			FloatType(Bitness bitness) : Type(Kind::FLOATTYPE), bitness(bitness) {}
			static bool classof(const Entity *e) {
				return e->getKind() == Kind::FLOATTYPE;
			}
			void visit(SemaEntityVisitor &v) override { v.visit(*this); }
			const Bitness bitness;
		};
		struct StringType : Type {
			StringType() : Type(Kind::STRINGTYPE) {}
			static bool classof(const Entity *e) {
				return e->getKind() == Kind::STRINGTYPE;
			}
			void visit(SemaEntityVisitor &v) override { v.visit(*this); }
		};
		struct FunctionType : Type {
			struct Argument { std::string name; Type *type = nullptr; Entity *defaultVal = nullptr; };
			FunctionType(std::vector<Argument> &&arguments, std::vector<Argument> &&results, bool isVarArg = false) :
				Type(Kind::FUNCTIONTYPE), arguments(std::move(arguments)), results(results), isVarArg(isVarArg)
			{}
			static bool classof(const Entity *e) {
				return e->getKind() == Kind::FUNCTIONTYPE;
			}
			void visit(SemaEntityVisitor &v) override { v.visit(*this); }

			std::vector<Argument> arguments;
			std::vector<Argument> results;
			bool isVarArg;
		};
		struct PointerType : Type {
			PointerType(Type *elementType) : Type(Kind::POINTERTYPE), elementType(elementType) {}
			static bool classof(const Entity *e) {
				return e->getKind() == Kind::POINTERTYPE;
			}
			void visit(SemaEntityVisitor &v) override { v.visit(*this); }

			Type *elementType;
		};
		struct StructType : Type {
			StructType() : Type(Kind::STRUCTTYPE) { }
			static bool classof(const Entity *e) {
				return e->getKind() == Kind::STRUCTTYPE;
			}
			void visit(SemaEntityVisitor &v) override { v.visit(*this); }

			struct Attribute {
				std::string name;
				Type *type = nullptr;
			};
			std::vector<Attribute> attributes;
			Scope *scope = nullptr;
		};
		struct TypeSymbol : Type {
			TypeSymbol(const std::string &name) : Type(Kind::TYPESYMBOL), name(name) { }
			static bool classof(const Entity *e) {
				return e->getKind() == Kind::TYPESYMBOL;
			}
			void visit(SemaEntityVisitor &v) override { v.visit(*this); }

			std::string name;
			Type *resolvedType = nullptr;
		};
		struct DependentType : Type {
			DependentType(Entity *dependency) : Type(Kind::DEPENDENTTYPE), dependency(dependency) { }
			static bool classof(const Entity *e) {
				return e->getKind() == Kind::DEPENDENTTYPE;
			}
			void visit(SemaEntityVisitor &v) override { v.visit(*this); }

			Entity *dependency = nullptr;
		};
	}
}
#endif
