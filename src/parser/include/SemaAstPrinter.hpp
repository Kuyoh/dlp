#ifndef DLP_PARSER_SEMAASTPRINTER_HPP
#define DLP_PARSER_SEMAASTPRINTER_HPP

#include <vector>
#include "SemaProgram.hpp"

namespace dlp {
	void printAst(std::ostream &o, sema::Program &p);
}
#endif
