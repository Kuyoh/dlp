#ifndef DLP_PARSER_SEMAEXPRESSION_HPP
#define DLP_PARSER_SEMAEXPRESSION_HPP

#include <list>
#include <vector>
#include "SemaTypes.hpp"

namespace llvm {
	struct Function;
	struct Value;
}

namespace dlp {
	namespace sema {
		struct Scope;
		// Functions
		struct IFunction : Entity {
			static bool classof(const Entity *e) {
				return e->getKind() >= Kind::FUNCTION && e->getKind() <= Kind::IMPORTEDFUNCTION;
			}

		protected:
			IFunction(Kind kind, FunctionType *type) : Entity(kind) { this->type = type; }
		};
		struct Function : IFunction {
			Function(FunctionType *type) : IFunction(Kind::FUNCTION, type) {}
			void visit(SemaEntityVisitor &v) override { v.visit(*this); }			
			static bool classof(const Entity *e) {
				return e->getKind() == Kind::FUNCTION;
			}
			Scope *scope;
			llvm::Function *llvmFunction;
		};
		struct ImportedFunction : IFunction {
			ImportedFunction(FunctionType *type) : IFunction(Kind::IMPORTEDFUNCTION, type) {}
			void visit(SemaEntityVisitor &v) override { v.visit(*this); }			
			static bool classof(const Entity *e) {
				return e->getKind() == Kind::IMPORTEDFUNCTION;
			}
			Entity *libName = nullptr;
			Entity *funcName = nullptr;
		};
		// Expression
		struct CallExpr : Entity {
			CallExpr(std::string funcName) :
				Entity(Kind::CALL), funcName(funcName) {
			}
			void visit(SemaEntityVisitor &v) override { v.visit(*this); }			
			static bool classof(const Entity *e) {
				return e->getKind() == Kind::CALL;
			}
			std::string funcName;
			IFunction *func = nullptr;
			Entity *funcEntity = nullptr;
			std::vector<Entity*> arguments;
		};
		struct MemAccessExpr : Entity {
			MemAccessExpr(std::string memName) :
				Entity(Kind::MEMACCESS), memName(memName) {
			}
			void visit(SemaEntityVisitor &v) override { v.visit(*this); }			
			static bool classof(const Entity *e) {
				return e->getKind() == Kind::MEMACCESS;
			}
			std::string memName;
			Entity *object = nullptr;
		};
	}
}
#endif
