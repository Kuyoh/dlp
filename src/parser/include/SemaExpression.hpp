#pragma once
#include <list>
#include <vector>
#include "SemaTypes.hpp"

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

		struct Symbol : Entity {
			Symbol(std::string name) :
				Entity(Kind::SYMBOL), name(name) {
			}
			void visit(SemaEntityVisitor &v) override { v.visit(*this); }			
			static bool classof(const Entity *e) {
				return e->getKind() == Kind::SYMBOL;
			}
			std::string name;
			Entity *entity = nullptr;
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
			Function *func = nullptr;
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