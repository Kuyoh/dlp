#pragma once
#include <vector>
#include "SemaEntityVisitor.hpp"

namespace dlp {
	namespace sema {
		struct Type;
		struct Entity {
			virtual ~Entity() = default;
			enum class Kind {
				// Functions
				FUNCTION,
				IMPORTEDFUNCTION,
				//OVERLOADEDFUNCTION,
				OVERLOADEDENTITY,

				// Values
				ARGUMENT,
				VARIABLE,
				CONSTANT,
				
				LITERALINT,
				LITERALDOUBLE,
				LITERALSTRING,

				// Expressions
				CALL,
				//INDEX, //index should also be a call
				MEMACCESS,
				SYMBOL,
				
				// Types
				TYPETYPE,
				BOOLTYPE,
				INTTYPE,
				FLOATTYPE,
				STRINGTYPE,
				POINTERTYPE,
				FUNCTIONTYPE,
				STRUCTTYPE,
				TYPESYMBOL,
				DEPENDENTTYPE
			};
			Kind getKind() const { return kind; }
			Type *type = nullptr;
			virtual void visit(SemaEntityVisitor &v) = 0;
		protected:
			Entity(Kind kind) : kind(kind) {}
			const Kind kind;
		};

		// Values
		struct Argument : Entity {
			Argument(std::string name, Type *type = nullptr) : Entity(Kind::ARGUMENT), name(name) { this->type = type; }
			void visit(SemaEntityVisitor &v) override { v.visit(*this); }
			static bool classof(const Entity *e) {
				return e->getKind() == Kind::ARGUMENT;
			}
			std::string name;
		};
		struct Variable : Entity {
			Variable(std::string name) : Entity(Kind::VARIABLE), name(name) {}
			void visit(SemaEntityVisitor &v) override { v.visit(*this); }
			static bool classof(const Entity *e) {
				return e->getKind() == Kind::VARIABLE;
			}
			std::string name;
		};
		struct Constant : Entity {
			Constant(std::string name) : Entity(Kind::CONSTANT), name(name) {}
			void visit(SemaEntityVisitor &v) override { v.visit(*this); }
			static bool classof(const Entity *e) {
				return e->getKind() == Kind::CONSTANT;
			}
			std::string name;
			Entity *expr = nullptr;
		};

		// Constants
		struct LiteralInt : Entity {
			LiteralInt(uint64_t value, Type *type) : Entity(Kind::LITERALINT), value(value) {
				this->type = type;
			}
			void visit(SemaEntityVisitor &v) override { v.visit(*this); }
			static bool classof(const Entity *e) {
				return e->getKind() == Kind::LITERALINT;
			}
			const uint64_t value;
		};
		struct LiteralFloat : Entity {
			LiteralFloat(double value, Type *type) : Entity(Kind::LITERALDOUBLE), value(value) {
				this->type = type;
			}
			void visit(SemaEntityVisitor &v) override { v.visit(*this); }
			static bool classof(const Entity *e) {
				return e->getKind() == Kind::LITERALDOUBLE;
			}
			const double value;
		};
		struct LiteralString : Entity {
			LiteralString(const std::string &value, Type *type) : Entity(Kind::LITERALSTRING), value(value) {
				this->type = type;
			}
			void visit(SemaEntityVisitor &v) override { v.visit(*this); }
			static bool classof(const Entity *e) {
				return e->getKind() == Kind::LITERALSTRING;
			}
			const std::string value;
		};

		struct OverloadedEntity : Entity {
			OverloadedEntity() : Entity(Kind::OVERLOADEDENTITY) {}
			void visit(SemaEntityVisitor &v) override { v.visit(*this); }
			static bool classof(const Entity *e) {
				return e->getKind() == Kind::OVERLOADEDENTITY;
			}
			std::vector<Entity*> entities;
			std::vector<FunctionType*> overloads;
		};
	}
}