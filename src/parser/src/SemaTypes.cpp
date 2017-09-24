#include "SemaTypes.hpp"

using namespace dlp;
using namespace dlp::sema;

TypeType typeTypeInstance;
Type *Type::typeType = &typeTypeInstance;
