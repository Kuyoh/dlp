#include "AstPrintingContext.hpp"
#include <algorithm>
#include <iterator>

using namespace dlp;

void PrintingContext::writeIndent() {
	std::fill_n(std::ostream_iterator<char>(o), (std::max)(0, indent), '\t');
}

void PrintingContext::startNode(const char *name) {
	writeIndent();
	o << name << " [" << std::endl;
	indent++;
}

void PrintingContext::endNode() {
	indent--;
	writeIndent();
	o << "]" << std::endl;
}

const char *PrintingContext::unOpToken2Str(UnaryOperatorType token) {
	switch (token) {
	case UnaryOperatorType::MINUS: return "-";
	default: return "unknown_token";
	}
}

const char *PrintingContext::binOpToken2Str(BinaryOperatorType token) {
	switch (token) {
	case BinaryOperatorType::COMP_EQ: return "==";
	case BinaryOperatorType::COMP_NEQ: return "!=";
	case BinaryOperatorType::COMP_LT: return "<";
	case BinaryOperatorType::COMP_LE: return "<=";
	case BinaryOperatorType::COMP_GT: return ">";
	case BinaryOperatorType::COMP_GE: return ">=";
	case BinaryOperatorType::PLUS: return "+";
	case BinaryOperatorType::MINUS: return "-";
	case BinaryOperatorType::MUL: return "*";
	case BinaryOperatorType::DIV: return "/";
	default: return "unknown_token";
	}
}
