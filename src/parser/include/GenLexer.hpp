#pragma once

#if !defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#include "parser.hpp"

namespace dlp {
	class Lexer : public yyFlexLexer {
	public:
		Lexer(std::istream *in, std::string *filename) : yyFlexLexer(in), filename(filename)
		{
		}

		int yylex(dlp::Parser::semantic_type *lval, dlp::Parser::location_type *lloc);

		void error(const char *str) {
			std::cerr << "Lexer Error:" << str;
		}
	private:
		std::string *filename;
		Parser::semantic_type *yylval = nullptr;
		Parser::location_type *yylloc = nullptr;
	};
}
