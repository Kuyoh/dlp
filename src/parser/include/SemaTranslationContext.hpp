#pragma once
#include <memory>
#include <cassert>
#include <iostream>
#include "SemaProgram.hpp"
#include "Builtin.hpp"

namespace dlp {
	namespace sema {
		struct SemaTranslationContext {
			std::unique_ptr<Program> program = std::make_unique<Program>();
			Scope *currentScope = program.get();

			void logError(const char *msg) {
				std::cerr << msg;
			}
			Scope *pushScope() {
				Scope *result;
				currentScope->children.emplace_back(result = new Scope(currentScope));
				return result;
			}
			void popScope() {
				assert(currentScope->parent != nullptr);
				currentScope = currentScope->parent;
			}
			Scope::EntitySlot &nameEntity(const std::string &name) {
				return currentScope->namedEntities[name];
			}
			Entity *resolveEntity(const std::string &name) {
				Scope *s = currentScope;
				while (s != nullptr) {
					auto it = s->namedEntities.find(name);
					if (it != s->namedEntities.end())
						return it->second.entity;
					s = s->parent;
				}
				return resolveBuiltin(name);
			}

			template <typename T, typename... Args>
			T *addStatement(Args&&... args) {
				T *result;
				currentScope->statements.emplace_back(result = new T(args...));	
				return result;
			}

			template <typename T, typename... Args>
			T *create(Args&&... args) {
				T *result;
				currentScope->entities.emplace_back(result = new T(std::forward<Args>(args)...));
				return result;
			}
		};
	}
}