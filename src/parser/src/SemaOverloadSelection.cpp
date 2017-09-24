#include "SemaOverloadSelection.hpp"

using namespace dlp::sema;

int dlp::sema::selectOverload(std::vector<Type*> &argTypes, std::vector<FunctionType*> &overloads) {
	// we need information on available conversion ops -> implement this in translation context instead???
	return 0;
}
