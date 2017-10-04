#ifndef DLP_PARSER_BUILTIN_HPP
#define DLP_PARSER_BUILTIN_HPP

#include "SemaTypes.hpp"

namespace dlp {
	namespace sema {
		Entity *resolveBuiltin(const std::string &name);
	}
}
#endif
