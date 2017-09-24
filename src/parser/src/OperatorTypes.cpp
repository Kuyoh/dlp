#include "OperatorTypes.hpp"

using namespace dlp;

const char *dlp::unOpToken2Str(UnaryOperatorType token) {
	switch (token) {
	case UnaryOperatorType::MINUS: return "-";
	default: return nullptr;
	}
}

const char *dlp::binOpToken2Str(BinaryOperatorType token) {
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
	default: return nullptr;
	}
}