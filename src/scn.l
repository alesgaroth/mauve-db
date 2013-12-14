
%{
#include "sqlparser.h"
#define yylval (*((struct yydata *)yyextra))
%}

%option nounput noyywrap noinput
%%
CREATE { return CREATE; }
DATABASE { return DATABASE ; }
DROP { return DROP; }

[;] { return yytext[0];}
[A-Za-z][A-Za-z0-9_]* { yylval.strval = yytext; return NAME; }

[ \t\n\r\f\v]+ /* whitespace */
