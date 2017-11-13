#ifndef DLP_PARSER_SEMAENTITY_HPP
#define DLP_PARSER_SEMAENTITY_HPP

#include <vector>
#include "SemaEntityVisitor.hpp"

namespace llvm {
	class Value;
	class Type;
}

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
		struct Constant : Entity {
			Constant(std::string name) : Entity(Kind::CONSTANT), name(name) {}
			void visit(SemaEntityVisitor &v) override { v.visit(*this); }
			static bool classof(const Entity *e) {
				return e->getKind() == Kind::CONSTANT;
			}
			std::string name;
			Entity *expr = nullptr;
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
			//Entity *entity = nullptr;
			llvm::Value *llvmAddress = nullptr;
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
			llvm::Value *llvmValue = nullptr;
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
			llvm::Value *llvmValue = nullptr;
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
			llvm::Value *llvmValue = nullptr;
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
#endif
