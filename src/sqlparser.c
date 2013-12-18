#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "sqlparser.h"
#include <strings.h>
#include <string.h>
#include "memman.h"
#include "rbtree.h"


TreeNode createNewDatabase(const char *strval, TreeNode tn);
TreeNode dropDatabase(const char *strval, TreeNode tn);
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
	if (strcmp(username, "root") || strcmp(password, "password")){
		return 1;
	}
	struct yydata data = {NULL};
	yyscan_t scandata;
	if(yylex_init_extra(&data,&scandata)){
		perror("yylex_init_extra:");
		exit(3);
	}
	TreeNode tn = NULL;
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
							tn = createNewDatabase(data.strval, tn);
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
							tn = dropDatabase(data.strval, tn);
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
	return 0;
}
int report(yyscan_t scandata, const char *note){
	fprintf(stderr, "%s:%d:%d parser error %s %s\n",
		"stdin", yyget_lineno(scandata), yyget_column(scandata),
		note, yyget_text(scandata));
	return 1;
}

struct database {
	long size; 
	const char *name;
};

const char *string_dup(const char *val){
	int len = strlen(val)+2;
	assert(len < 127 || "don't support long string yet");
	char *d = heap_alloc(len);
	d[0] = len;
	memcpy(d+1, val, len+1);
	return d+1;
}

int sign(int v){
	return (((v)==0)?0:(((v)<0)?-1:1));
}

int RowMatcher(Row r_, Closure c){
	struct database *r = (struct database*)r_;
	struct database *mr = (struct database*)c;
	return sign(strcasecmp(r->name, mr->name));
}

TreeNode createNewDatabase(const char *strval, TreeNode tn){
	strval = string_dup(strval);
	struct database * db = heap_alloc(sizeof(*db));
	db->size = sizeof(*db)/sizeof(long);
	db->name = strval;
	struct lookUp lu = { RowMatcher, db};
	if(tree_findSingle(tn, &lu)){
		fprintf(stderr, "Error: Database %s already exists\n", strval);
		exit(1);
	}
	return tree_insert(tn, db, &lu);
}
TreeNode dropDatabase(const char *strval, TreeNode tn){
	struct database db = {0, strval};
	struct lookUp lu = (struct lookUp){ RowMatcher, &db};
	if(!tree_findSingle(tn, &lu)){
		fprintf(stderr, "Error: Database %s does not exist\n", strval);
		exit(1);
	}
	return tree_deleteSingle(tn, &lu);
}
