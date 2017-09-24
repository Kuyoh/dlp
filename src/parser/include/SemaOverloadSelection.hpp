#pragma once
#include "SemaTypes.hpp"

namespace dlp {
	namespace sema {
		int selectOverload(std::vector<Type*> &argTypes, std::vector<FunctionType*> &overloads);
	}
}