#ifndef DLP_PARSER_OPERATORTYPES_HPP
#define DLP_PARSER_OPERATORTYPES_HPP

namespace dlp {
	enum class UnaryOperatorType {
		MINUS
	};

	enum class BinaryOperatorType {
		PLUS,
		MINUS,
		MUL,
		DIV,
		COMP_EQ,
		COMP_NEQ,
		COMP_LT,
		COMP_LE,
		COMP_GT,
		COMP_GE
	};

	const char *unOpToken2Str(UnaryOperatorType type);
	const char *binOpToken2Str(BinaryOperatorType type);
}
#endif
