
%{
#include "sqlparser.h"
#define yylval (*((struct yydata *)yyextra))
%}

%option nounput noyywrap noinput
%%
CREATE { return CREATE; }
DATABASE { return DATABASE ; }
DROP { return DROP; }
USE { return USE; }
TABLE { return TABLE; }
INTEGER { return INTEGER; }
NOT { return NOT; }
NULL { return NULLX; }
VARCHAR { return VARCHAR; }
PRIMARY { return PRIMARY; }
KEY { return KEY; }
AUTO_INCREMENT { return AUTO_INCREMENT; }

[;)(,.] { return yytext[0];}
[A-Za-z][A-Za-z0-9_]* { yylval.strval = yytext; return NAME; }
[0-9][0-9]* { yylval.strval = yytext; return NUMBER; }

[\n] { yylineno ++ ; }
[ \t\r\f\v]+ /* whitespace */

