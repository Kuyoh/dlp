%skeleton "lalr1.cc"
%require "2.5"
%debug
%defines
%define namespace "dlp"
%define parser_class_name "Parser"

%locations
%glr-parser

%code requires{
	#include "ast.hpp"
	#include <vector>

	namespace dlp {
		class Lexer;
		class Parser;
	}
}

%lex-param   { Lexer  &scanner  }
%parse-param { Lexer  &scanner  }

%lex-param   { std::unique_ptr<dlp::ast::StatementList> *program  }
%parse-param { std::unique_ptr<dlp::ast::StatementList> *program  }

%code{
	#include <iostream>
	#include "DlpParser.hpp"

	using namespace dlp;
	using namespace dlp::ast;
	static int yylex(Parser::semantic_type *yylval, Parser::location_type *yylloc, Lexer &scanner, std::unique_ptr<dlp::ast::StatementList> *program);
}

/* Represents the many different ways we can access our data */
%union {
	ast::INode *node;
	ast::IBlock *block;
	ast::IType *type;
	ast::IExpression *expr;
	ast::IStatement *stmt;
	ast::StatementList *stmts;
	ast::Identifier *ident;
	//ast::CommandStatement *command;
	ast::VariableDefinition *var_def;
	//ast::MethodCall *method_call;
	ast::VariableList *varvec;
	ast::ExpressionList *exprvec;
	//ast::StringList *identvec;
	std::string *string;
	int token;
}

/* terminals */
%token <string> TIDENTIFIER THEXINT TOCTINT TDECINT TDOUBLE TSTRING
%token <token> TCEQ TCNE TCLT TCLE TCGT TCGE TEQUAL
%token <token> TLPAREN TRPAREN TLBRACE TRBRACE TLBRACKET TRBRACKET TCOMMA TDOT TARROW TCOLON TSEMICOLON TELLIPSIS
%token <token> TPLUS TMINUS TMUL TDIV
%token <token> TRETURN TEXTERN
%token <token> TIF TELSE TELSEIF
%token <token> TUSING TYIELD TDEFER
// defer vs onFail??
%token <token> TLOOP TCONTINUE TBREAK
%token <token> TSWITCH TCASE
%token END 0 "end of file"

/* nonterminal node types */
%type <expr> literal expr
//non_call_expr
%type <varvec> func_type_args func_type_args_novaarg
%type <exprvec> expr_list
%type <type> type func_type
//%type <command> command commandchain
%type <ident> ident
//%type <method_call> method_call
%type <block> block stmt_or_block
%type <stmts> stmts
%type <stmt> stmt if_stmt_tail for_stmt_tail
%type <var_def> var_def
%destructor { } <token> 
%destructor { delete $$; } <*>
//%destructor { delete $$; } <expr> 
//%destructor { delete $$; } <varvec>
//%destructor { delete $$; } <exprvec>
//%destructor { delete $$; } <type>
//%destructor { delete $$; } <block>
//%destructor { delete $$; } <stmts>
//%destructor { delete $$; } <stmt>
//%destructor { delete $$; } <var_def>

/* Operator precedence for logical operators */
%nonassoc TCEQ TCNE TCLT TCLE TCGT TCGE TEQUAL

/* Operator precedence for arithmetical operators */
%left TPLUS TMINUS
%left TMUL TDIV
%left NEG
%left TDOT

%nonassoc "then"
%nonassoc TELSEIF
%nonassoc TELSE

%start program

/*
for scopeing take a look at clang:
	https://clang.llvm.org/doxygen/classclang_1_1Scope.html#a11d6331901f84b04757fb829168ee861

scoping is definetly part of the semantics, not the syntax!

check output file using:
.\win_bison.exe --output="parser.cpp" --defines="parser.hpp"  "parser.y" --report=state

*/
%%

/*
we need to separate type and identifiers!!!
=> maybe simple allow identifier as both type and expression (need to adapt AST)?
*/
program : 
		  | stmts { program->reset($1); $1 = nullptr; }
		  ;
		  
block : TLBRACE stmts TRBRACE { $$ = new Block($2); }
	  | TLBRACE TRBRACE { $$ = new Block(); }
	  | TEXTERN TLPAREN expr TCOMMA expr TRPAREN { $$ = new ExternBlock($3, $5); }
	  ;

stmt_or_block : block { $$ = $1; }
			  | stmt { auto *s = new StatementList(); s->emplace_back($1); $$ = new Block(s); }

stmts : stmt { $$ = new StatementList(); $$->emplace_back($1); }
	  | stmts stmt { $1->emplace_back($2); $$ = $1; }
	  ;

if_stmt_tail : expr stmt_or_block %prec "then" { $$ = new IfStatement($1, $2); }
			 | expr stmt_or_block TELSE stmt_or_block { $$ = new IfStatement($1, $2, $4); }
			 | expr stmt_or_block TELSEIF if_stmt_tail { auto *s = new StatementList(); s->emplace_back($4); $$ = new IfStatement($1, $2, new Block(s)); }
			 ;
			 
for_stmt_tail : expr stmt_or_block { $$ = new ForStatement($1, $2); }
			  ;

// TODO: allow type specification in constants definitions!
stmt : var_def TSEMICOLON { $$ = $1; }
	 | TLOOP for_stmt_tail { $$ = $2; }
	 | TIF if_stmt_tail { $$ = $2; }
	 | TIDENTIFIER TCOLON TCOLON func_type block { $$ = new ConstantDefinition($1, new DefinitionExpression($4, $5)); }
	 | TIDENTIFIER TCOLON TCOLON ident block { $$ = new ConstantDefinition($1, new DefinitionExpression($4, $5)); }
	 | TIDENTIFIER TCOLON TCOLON expr TSEMICOLON { $$ = new ConstantDefinition($1, $4); }
	 | TIDENTIFIER TEQUAL expr TSEMICOLON { $$ = new Assignment($1, $3); }
	 | expr TSEMICOLON { $$ = new ExpressionStatement($1); }
	 | TRETURN expr TSEMICOLON { $$ = new ReturnStatement($2); }
	 | block { $$ = $1; }
     ;

var_def : TIDENTIFIER TCOLON type { $$ = new VariableDefinition($3, $1); }
		 | TIDENTIFIER TCOLON type TEQUAL expr { $$ = new VariableDefinition($3, $1, $5); }
		 | TIDENTIFIER TCOLON TEQUAL expr { $$ = new VariableDefinition(nullptr, $1, $4); }
		 ;
		 
func_type_args : func_type_args_novaarg
			   | func_type_args_novaarg TCOMMA TELLIPSIS { $1->isVarArg = true; $$ = $1; }
			   ;

func_type_args_novaarg : /* no arguments */  { $$ = new VariableList(); }
					   | var_def { $$ = new VariableList(); $$->emplace_back($1); }
					   | func_type_args_novaarg TCOMMA var_def { $1->emplace_back($3); }
					   ;

ident : TIDENTIFIER { $$ = new Identifier($1); }
	  ;

// does not work since we introduced type, because this ident cannot be distinguished from an ident in an expression!
// maybe it is easier to solve if we put ambiguous grammas into a single rule
type : ident { $$ = $1; }
	 | func_type { $$ = $1; }
	 | type TMUL { $$ = new PointerType($1); }
	 ;

func_type : TLPAREN func_type_args TRPAREN { $$ = new FunctionType($2); }
		  | TLPAREN func_type_args TRPAREN TARROW TLPAREN func_type_args_novaarg TRPAREN { $$ = new FunctionType($2, $6); }
		  ;

literal : THEXINT TIDENTIFIER { $$ = new HexLiteral($1, $2); }
		| THEXINT { $$ = new HexLiteral($1); }
		| TOCTINT TIDENTIFIER { $$ = new OctalLiteral($1, $2); }
		| TOCTINT { $$ = new OctalLiteral($1); }
		| TDECINT TIDENTIFIER { $$ = new DecimalLiteral($1, $2); }
		| TDECINT { $$ = new DecimalLiteral($1); }
		| TDOUBLE TIDENTIFIER { $$ = new FloatLiteral($1, $2); }
		| TDOUBLE { $$ = new FloatLiteral($1); }
		| TSTRING TIDENTIFIER { $$ = new StringLiteral($1, $2); }
		| TSTRING { $$ = new StringLiteral($1); }
		;

		/*
method_call : TIDENTIFIER TLPAREN expr_list TRPAREN { $$ = new MethodCall($1, $3); }
			;

expr : method_call { $$ = $1; }
	 | non_call_expr { $$ = $1; }
	 ;
	 */

expr : ident { $$ = $1; }
	 | TIDENTIFIER TLPAREN expr_list TRPAREN { $$ = new MethodCall($1, $3); }
	 | expr TDOT TIDENTIFIER { $$ = new MemberAccess($1, $3); }
	 | expr TLBRACKET expr_list TRBRACKET { $$ = new IndexAccess($1, $3); }
	 | literal
         | expr TMUL expr { $$ = new BinaryOperator($1, dlp::BinaryOperatorType::MUL, $3); }
         | expr TDIV expr { $$ = new BinaryOperator($1, dlp::BinaryOperatorType::DIV, $3); }
         | expr TPLUS expr { $$ = new BinaryOperator($1, dlp::BinaryOperatorType::PLUS, $3); }
         | expr TMINUS expr { $$ = new BinaryOperator($1, dlp::BinaryOperatorType::MINUS, $3); }
         | TMINUS expr %prec NEG { $$ = new UnaryOperator($2, dlp::UnaryOperatorType::MINUS); }
 	 | expr TCEQ expr { $$ = new BinaryOperator($1, dlp::BinaryOperatorType::COMP_EQ, $3); }
 	 | expr TCNE expr { $$ = new BinaryOperator($1, dlp::BinaryOperatorType::COMP_NEQ, $3); }
 	 | expr TCLT expr { $$ = new BinaryOperator($1, dlp::BinaryOperatorType::COMP_LT, $3); }
 	 | expr TCLE expr { $$ = new BinaryOperator($1, dlp::BinaryOperatorType::COMP_LE, $3); }
 	 | expr TCGT expr { $$ = new BinaryOperator($1, dlp::BinaryOperatorType::COMP_GT, $3); }
 	 | expr TCGE expr { $$ = new BinaryOperator($1, dlp::BinaryOperatorType::COMP_GE, $3); }
     | TLPAREN expr TRPAREN { $$ = $2; }
	 | ident block { new DefinitionExpression($1, $2); }
	 | func_type block { new DefinitionExpression($1, $2); }
	 ;
	 //| ident TARROW block { new DefinitionExpression($1, $2); } // lambda - we will define that later
	 //| TLPAREN identlist TRPAREN TARROW block { new DefinitionExpression($1, $2); }
	 //;

//identlist :  /*blank*/  { $$ = new StringList(); }
//		  | TIDENTIFIER { $$ = new StringList(); $$->emplace_back($1); }
//		  | identlist TCOMMA TIDENTIFIER  { $1->emplace_back($3); }

expr_list : /*blank*/  { $$ = new ExpressionList(); }
		  | expr { $$ = new ExpressionList(); $$->emplace_back($1); }
		  | expr_list TCOMMA expr  { $1->emplace_back($3); }
		  ;

%%

void Parser::error( const dlp::Parser::location_type &l,
                           const std::string &err_message)
{
	if (l.begin.filename != nullptr)
		std::cerr << *(l.begin.filename) << "(" << l.begin.line << "," << l.begin.column << ") Error: " << err_message << std::endl; 
	else
		std::cerr << "Line " << l.begin.line << ", Column " << l.begin.column << " Error: " << err_message << std::endl; 
}


/* include for access to scanner.yylex */
#include "GenLexer.hpp"
static int yylex(Parser::semantic_type *yylval, Parser::location_type *yylloc,
                 Lexer  &scanner, std::unique_ptr<dlp::ast::StatementList> *program)
{
   return( scanner.yylex(yylval, yylloc) );
}

static dlp::ast::INode::Location loc(dlp::Parser::location_type *l) {
	return dlp::ast::INode::Location { l->begin.filename != nullptr ? l->begin.filename : nullptr, l->begin.line, l->end.line, l->begin.column, l->end.column };
}
