#ifndef DLP_PARSER_SEMAOVERLOADSELECTION_HPP
#define DLP_PARSER_SEMAOVERLOADSELECTION_HPP

#include "SemaTypes.hpp"

namespace dlp {
	namespace sema {
		int selectOverload(std::vector<Type*> &argTypes, std::vector<FunctionType*> &overloads);
	}
}
#endif
