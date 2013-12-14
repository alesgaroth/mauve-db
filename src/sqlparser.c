#include <stdio.h>
#include <stdlib.h>
#include "sqlparser.h"

void createNewDatabase(char *strval);
void dropDatabase(char *strval);
int report(yyscan_t scandata, const char*note);

int main(int argc, char **argv){
	int j;
	const char *username = NULL, *password = NULL, *expression = NULL;
	for(j = 1; j <argc; j += 1){
		if(argv[j][0] == '-'){
			switch(argv[j][1]){
			case 'u':
				username = (argv[j][2])?argv[j]+2:argv[++j];
				break;
			case 'p':
				password = (argv[j][2])?argv[j]+2:argv[++j];
				break;
			case 'e':
				expression = (argv[j][2])?argv[j]+2:argv[++j];
				break;
			}
		}
	}
	struct yydata data = {NULL};
	yyscan_t scandata;
	if(yylex_init_extra(&data,&scandata)){
		perror("yylex_init_extra:");
		exit(3);
	}
	if (expression){
	}else {
		int parseval;
		while((parseval  = yylex(scandata))){
			switch(parseval){
			case CREATE:
				switch(yylex(scandata)){
				case YY_NULL: return report(scandata, "unexpected end of file");
				case DATABASE:
					switch(yylex(scandata)){
					case YY_NULL:  return report(scandata, "unexpected end of file");
					case NAME:
						switch(yylex(scandata)){
						case YY_NULL:  return report(scandata, "unexpected end of file");
						case ';':
							createNewDatabase(data.strval);
							break;
						default: return report(scandata, "Unexpected token:");
						}
						break;
					default: return report(scandata, "Unexpected token:");
					}
					break;
				default: return report(scandata, "uNexpected token:");
				}
				break;
			case DROP:
				switch(yylex(scandata)){
				case YY_NULL: return report(scandata, "unexpected end of file");
				case DATABASE:
					switch(yylex(scandata)){
					case YY_NULL: return report(scandata, "unexpected end of file");
					case NAME:
						switch(yylex(scandata)){
						case YY_NULL:  return report(scandata, "unexpected end of file");
						case ';':
							dropDatabase(data.strval);
							break;
						default: return report(scandata, "Unexpected token:");
						}
						break;
					default: return report(scandata, "unExpected token:");
					}
					break;
				default: return report(scandata, "uneXpected token:");
				}
				break;
			default: return report(scandata, "unexPected token:");
			}
		}
	}
	yylex_destroy(scandata);
	scandata = NULL;
}
int report(yyscan_t scandata, const char *note){
	fprintf(stderr, "%s:%d:%d parser error %s %s\n",
		"stdin", yyget_lineno(scandata), yyget_column(scandata),
		note, yyget_text(scandata));
	return 1;
}

void createNewDatabase(char *strval){
	
}
void dropDatabase(char *strval){
	
}
