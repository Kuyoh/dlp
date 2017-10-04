#ifndef DLP_PARSER_ASTPRINTINCONTEXT_HPP
#define DLP_PARSER_ASTPRINTINCONTEXT_HPP

#include <ostream>
#include "OperatorTypes.hpp"

namespace dlp {
	struct PrintingContext {
		std::ostream &o;
		int indent = -1;

		PrintingContext(std::ostream &o) : o(o) {}
		
		void writeIndent();
		void startNode(const char *name);
		void endNode();

		const char *unOpToken2Str(UnaryOperatorType token);
		const char *binOpToken2Str(BinaryOperatorType token);

		template <typename T> void printNamedValue(const char *name, T &&value) {
			writeIndent();
			o << name << "[" << value << "]" << std::endl;
		}
		template <typename T> void printValue(T &&value) {
			writeIndent();
			o << value << std::endl;
		}
	};
}
#endif
