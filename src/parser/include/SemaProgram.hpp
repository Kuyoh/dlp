#ifndef DLP_PARSER_SEMAPROGRAM_HPP
#define DLP_PARSER_SEMAPROGRAM_HPP

#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include "SemaStatement.hpp"

namespace dlp {
	namespace sema {
		struct Scope {
			struct EntitySlot { Entity *entity; };
			std::list<Variable*> variables;
			std::unordered_map<std::string, EntitySlot> namedEntities;
			std::list<std::unique_ptr<Statement>> statements;
			std::list<std::unique_ptr<Entity>> entities;
			std::list<std::unique_ptr<Scope>> children;
			Scope *parent;
			Scope(Scope *parent = nullptr) : parent(parent) {}
		};
		using Program = Scope;
	}
}
#endif
