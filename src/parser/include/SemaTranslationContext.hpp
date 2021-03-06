#ifndef DLP_PARSER_SEMATRANSLATIONCONTEXT_HPP
#define DLP_PARSER_SEMATRANSLATIONCONTEXT_HPP

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
				currentScope = result;
				return result;
			}
			void popScope() {
				assert(currentScope->parent != nullptr);
				currentScope = currentScope->parent;
			}
			Scope::EntitySlot &nameEntity(const std::string &name) {
				return currentScope->namedEntities[name];
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
#endif
