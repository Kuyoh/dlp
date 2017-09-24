%{
#include <string>
#include "parser.hpp"
#include "GenLexer.hpp"

#undef  YY_DECL
#define YY_DECL int dlp::Lexer::yylex(dlp::Parser::semantic_type *lval, dlp::Parser::location_type *lloc)

#define yyterminate() return( dlp::Parser::token_type::END )

#define YY_USER_ACTION yylloc->step(); yylloc->columns(yyleng);

// #define YY_NO_UNISTD_H

#define SAVE_TOKEN  yylval->string = new std::string(yytext, yyleng)
#define SAVE_STRING yylval->string = new std::string(yytext+1, yyleng-2)
#define TOKEN(t)    (yylval->token = t)

// ideally we have no keywords at all
// we will probably need some anyway ...

// for now we use c keywords for the constructs we require

// http://www.jonathanbeard.io/tutorials/FlexBisonC++

//int line_num = 1;
//int comment_caller;
%}

%option debug
%option nodefault
%option yyclass="dlp::Lexer"
%option noyywrap
%option c++

%s MLCOMMENT
%s SLCOMMENT
%%
%{
	using t = dlp::Parser::token_type;
	yylval = lval;
	yylloc = lloc;
	yylloc->begin.filename = filename;
	yylloc->end.filename = filename;
/*
"async"							return TOKEN(t::TASYNC);
"while"							return TOKEN(t::TWHILE);
"do"							return TOKEN(t::TDO);
*/

/* might be an operator/function instead:
"in"							return TOKEN(t::TIN);
*/
%}

<INITIAL>"/*"				BEGIN(MLCOMMENT);
<MLCOMMENT>[^*\n]*			/* eat anything that's not a '*' */;
<MLCOMMENT>"*"+[^*/\n]*		/* eat up '*'s not followed by '/'s */;
<MLCOMMENT>\n				yylloc->lines(); 
<MLCOMMENT>"*"+"/"			BEGIN(INITIAL);
<MLCOMMENT><<EOF>>   {
         error( "unterminated comment" );
         yyterminate();
         }

<INITIAL>"//"[^\n]*			BEGIN(SLCOMMENT);
<SLCOMMENT>[^\n]*			/* eat anything that's not a '\n' */;
<SLCOMMENT>\n				yylloc->lines(); BEGIN(INITIAL);
<SLCOMMENT><<EOF>>			BEGIN(INITIAL);


<INITIAL>\n						yylloc->lines();
[ \t]					        ;

"extern"						return TOKEN(t::TEXTERN);
"return"				        return TOKEN(t::TRETURN);
"if"							return TOKEN(t::TIF);
"else"							return TOKEN(t::TELSE);
"elseif"						return TOKEN(t::TELSEIF);
"using"							return TOKEN(t::TUSING);
"loop"							return TOKEN(t::TLOOP);
"yield"							return TOKEN(t::TYIELD);
"defer"							return TOKEN(t::TDEFER);
"break"							return TOKEN(t::TBREAK);
"continue"						return TOKEN(t::TCONTINUE);
"switch"						return TOKEN(t::TSWITCH);
"case"							return TOKEN(t::TCASE);

[a-zA-Z_][a-zA-Z0-9_]*          SAVE_TOKEN; return t::TIDENTIFIER;
"0"[xX][0-9a-fA-F_]+			SAVE_TOKEN; return t::THEXINT;
"0"[0-7_]+						SAVE_TOKEN; return t::TOCTINT;
[0-9_]+							SAVE_TOKEN; return t::TDECINT;
[0-9_]+\.[0-9_]*				SAVE_TOKEN; return t::TDOUBLE;
\"(\\.|[^\"])*\"				SAVE_STRING; return t::TSTRING; // for now we exclude newline, because we would need to update yylloc -> when we are ready we need to use start conditions here

":" 							return TOKEN(t::TCOLON);
"=" 							return TOKEN(t::TEQUAL);
"=="							return TOKEN(t::TCEQ);
"!="							return TOKEN(t::TCNE);
"<" 							return TOKEN(t::TCLT);
"<="							return TOKEN(t::TCLE);
">" 							return TOKEN(t::TCGT);
">="							return TOKEN(t::TCGE);

"(" 							return TOKEN(t::TLPAREN);
")" 							return TOKEN(t::TRPAREN);
"{" 							return TOKEN(t::TLBRACE);
"}" 							return TOKEN(t::TRBRACE);
"["								return TOKEN(t::TLBRACKET);
"]"								return TOKEN(t::TRBRACKET);

"." 							return TOKEN(t::TDOT);
"," 							return TOKEN(t::TCOMMA);
";" 							return TOKEN(t::TSEMICOLON);
"..."							return TOKEN(t::TELLIPSIS);

"->"							return TOKEN(t::TARROW);

"+" 							return TOKEN(t::TPLUS);
"-" 							return TOKEN(t::TMINUS);
"*" 							return TOKEN(t::TMUL);
"/" 							return TOKEN(t::TDIV);

.   							printf("Unknown token!\n"); yyterminate();

%%
