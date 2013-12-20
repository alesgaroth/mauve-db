#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "sqlparser.h"
#include <strings.h>
#include <string.h>
#include "memman.h"
#include "rbtree.h"


TreeNode createIndexes(void);
TreeNode createNewDatabase(const char *strval, TreeNode tn);
TreeNode dropDatabase(const char *strval, TreeNode tn);
TreeNode createNewTable(const char *db, const char *tablename, TreeNode tn);
TreeNode dropTable(const char *db, const char *table, TreeNode tni);
TreeNode createNewColumn(const char *db, const char *tablename, const char *columnname,
	const char *type, int isprimarykey, int auto_increment, int notnullable, TreeNode tn);
int report(yyscan_t scandata, const char*note);
const char *string_dup(const char *val);

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
	TreeNode tn = NULL;
	tn = createIndexes();
	struct yydata data = {NULL};
	yyscan_t scandata;
	if(yylex_init_extra(&data,&scandata)){
		perror("yylex_init_extra:");
		exit(3);
	}
	if (expression){
	}else {
		int parseval;
		const char *current_db = NULL;
		while((parseval  = yylex(scandata))){
			switch(parseval){
			case ';':
				break;
			case USE:
				switch(yylex(scandata)){
				case YY_NULL:  return report(scandata, "unexpected end of file");
				case NAME:
					current_db = string_dup(data.strval);
					break;
				default: return report(scandata, "Unexpected token:");
				}
				break;
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
				case TABLE:
					switch(yylex(scandata)){
					case YY_NULL:  return report(scandata, "unexpected end of file");
					case NAME:{
						const char *db = current_db;
						const char *tablename = string_dup(data.strval);
						switch(yylex(scandata)){
						case YY_NULL:  return report(scandata, "unexpected end of file");
						case '.': 
							db = tablename;
							switch(yylex(scandata)){
							case YY_NULL:  return report(scandata, "unexpected end of file");
							case NAME:
								tablename = string_dup(data.strval);
								break;
							default: return report(scandata, "Unexpected token:");
							}
							switch(yylex(scandata)){
							case YY_NULL:  return report(scandata, "unexpected end of file");
							case '(':
								break;
							default: return report(scandata, "Unexpected token:");
							}
							/* fall through */
						case '(':
							tn = createNewTable(db, tablename, tn);
							int columnsdone = 0;
							do {
								switch(yylex(scandata)){
								case YY_NULL:  return report(scandata, "unexpected end of file");
								case ')': columnsdone = 1; break;
								case NAME:{
									const char * columnname = string_dup(data.strval);
									const char *type = NULL;
									int notnullable = 0;
									int isprimarykey = 0;
									int auto_increment = 0;
									switch(yylex(scandata)){
									case YY_NULL:  return report(scandata, "unexpected end of file");
									case VARCHAR:
										type = "VARCHAR";
									case INTEGER:
										if (type != NULL){
											type = "INTEGER";
										}
										int thiscoldone = 0;
										do {
											switch(yylex(scandata)){
											case YY_NULL:  return report(scandata, "unexpected end of file");
											case '(':
												switch(yylex(scandata)){
												case YY_NULL:  return report(scandata, "unexpected end of file");
												case NUMBER:
													switch(yylex(scandata)){
													case YY_NULL:  return report(scandata, "unexpected end of file");
													case ')':
														break;
													default: return report(scandata, "Unexpected token:");
													}
													break;

												default: return report(scandata, "Unexpected token:");
												}
												break;
												
											case ')': columnsdone = 1;/*fall through */
											case ',': thiscoldone = 1;break;
											case NOT:
												switch(yylex(scandata)){
												case YY_NULL:  return report(scandata, "unexpected end of file");
												case NULLX: notnullable = 1;
													break;
												default: return report(scandata, "Unexpected token:");
												}
												break;
											case PRIMARY:
												switch(yylex(scandata)){
												case YY_NULL:  return report(scandata, "unexpected end of file");
												case KEY: isprimarykey = 1;
													break;
												default: return report(scandata, "Unexpected token:");
												}
												break;
											case AUTO_INCREMENT:
												auto_increment = 1;
												break;
											default: return report(scandata, "Unexpected token:");
											}
										} while(!thiscoldone);
										tn = createNewColumn(db, tablename, columnname, type, isprimarykey, auto_increment, notnullable, tn);
										break;
									default: return report(scandata, "Unexpected token:");
									}
									} /*end case NAME*/
									break;
								default: return report(scandata, "Unexpected token:");
								}
							} while(!columnsdone);
							switch(yylex(scandata)){
							case YY_NULL:  return report(scandata, "unexpected end of file");
							case ';': break;
							default: return report(scandata, "Unexpected token:");
							}
						break;
						}
						}/*end CASE name:*/
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
				case TABLE:
					switch(yylex(scandata)){
					case YY_NULL: return report(scandata, "unexpected end of file");
					case NAME: {
						const char *tablename = string_dup(data.strval);
						const char *db = current_db;
						switch(yylex(scandata)){
						case YY_NULL:  return report(scandata, "unexpected end of file");
						case '.':
							db = tablename;
							switch(yylex(scandata)){
							case YY_NULL:  return report(scandata, "unexpected end of file");
							case NAME:
								tablename = string_dup(data.strval);
								break;
							default: return report(scandata, "Unexpected token:");
							}
							switch(yylex(scandata)){
							case YY_NULL:  return report(scandata, "unexpected end of file");
							case ';':
								break;
							default: return report(scandata, "Unexpected token:");
							}
							/* fall through */
						case ';':
							tn = dropTable(db, tablename, tn);
							break;
						default: return report(scandata, "Unexpected token:");
						}
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
	fprintf(stderr, "%s:%d:%d parser error %s '%s'\n",
		"stdin", yyget_lineno(scandata), yyget_column(scandata),
		note, yyget_text(scandata));
	return 1;
}

struct index {
	long size;
	const char *name;
	TreeNode index;
};

struct database {
	long size; 
	const char *aaa_ref;
	const char *name;
	const char *table_tree_ref;
};
struct table {
	long size;
	const char *aaa_ref;
	const char *database_name;
	const char *name;
	const char *database_ref;
	const char *column_tree_ref;
};
struct column {
	long size;
	const char *aaa_ref;
	const char *database_name;
	const char *table_name;
	const char *name;
	const char *type;
	const char *database_ref;
	const char *table_ref;
	long isprimarykey;
	long auto_increment;
	long notnullable;
};

struct closure {
	int numcols;
	Row samplerow;
	int colnums[10];
};

int sign(int v){
	return (((v)==0)?0:(((v)<0)?-1:1));
}
int RowMatcher(Row r_, Closure c_){
	struct closure*c = (struct closure*)c_;
	const char **r = (const char **)r_;
	int s = 0;
	int j;
	for(j = 0; s == 0 && j < c->numcols; j += 1){
		s = sign(strcasecmp(r[c->colnums[j]], ((const char **)c->samplerow)[c->colnums[j]]));
	}
	return s;
}


TreeNode createIndex(const char *val, TreeNode tn){
	struct index * ind = heap_alloc(sizeof(*ind));
	ind->size = sizeof(*ind)/sizeof(long);
	ind->name = val;
	ind->index = NULL;
	struct closure cl = {1, ind, {1}};
	struct lookUp lu = { RowMatcher, &cl};
	return tree_insert(tn, ind, &lu);
}

TreeNode createIndexes(void){
	TreeNode tn = createIndex("databases_index", NULL);
	tn = createIndex("tables_index", tn);
	tn = createIndex("columns_index", tn);
	return tn;
}
TreeNode findIndex(const char*name, TreeNode tn){
	struct index ind = {0, name, NULL};
	struct closure cl = {1, &ind, {1}};
	struct lookUp lu = { RowMatcher, &cl};
	struct index*ip =  tree_findSingle(tn, &lu);
	return ip->index;
}
TreeNode storeIndex(const char*name, TreeNode val, TreeNode tn){
	struct index ind = {sizeof(ind)/sizeof(long), name, val};
	struct index *ip = heap_alloc(sizeof(*ip));
	*ip = ind;
	struct closure cl = {1, ip, {1}};
	struct lookUp lu = { RowMatcher, &cl};
	return tree_update(tn, ip, &lu);
}

const char *string_dup(const char *val){
	int len = strlen(val)+2;
	assert(len < 127 || "don't support long string yet");
	char *d = heap_alloc(len);
	d[0] = len;
	memcpy(d+1, val, len+1);
	return d+1;
}


TreeNode createNewDatabase(const char *strval, TreeNode tni){
	TreeNode tn = findIndex("databases_index", tni);
	strval = string_dup(strval);
	struct database * db = heap_alloc(sizeof(*db));
	db->size = sizeof(*db)/sizeof(long);
	db->name = strval;
	struct closure cl = {1, db, {2}};
	struct lookUp lu = { RowMatcher, &cl};
	if(tree_findSingle(tn, &lu)){
		fprintf(stderr, "Error: Database %s already exists\n", strval);
		exit(1);
	}
	tn = tree_insert(tn, db, &lu);
	return storeIndex("databases_index", tn, tni);
}
TreeNode dropDatabase(const char *strval, TreeNode tni){
	TreeNode tn = findIndex("databases_index", tni);
	struct database db = {0, "", strval};
	struct closure cl = {1, &db, {2}};
	struct lookUp lu = (struct lookUp){ RowMatcher, &cl};
	if(!tree_findSingle(tn, &lu)){
		fprintf(stderr, "Error: Database %s does not exist\n",  strval);
		exit(1);
	}
	tn = tree_deleteSingle(tn, &lu);
	return storeIndex("databases_index", tn, tni);
}
TreeNode dropTable(const char *db, const char *tablename, TreeNode tni){
	TreeNode tn = findIndex("tables_index", tni);
	struct table t = {0, "", db, tablename};
	struct closure cl = {1, &t, {2, 3}};
	struct lookUp lu = (struct lookUp){ RowMatcher, &cl};
	if(!tree_findSingle(tn, &lu)){
		fprintf(stderr, "Error: Table %s.%s does not exist\n",  db, tablename);
		exit(1);
	}
	tn = tree_deleteSingle(tn, &lu);
	return storeIndex("tables_index", tn, tni);
}
TreeNode createNewTable(const char *db, const char *tablename, TreeNode tni){
	if (!db) {
		fprintf(stderr, "Error: No database specified\n");
		exit(1);
	}
	TreeNode tn = findIndex("tables_index", tni);
	struct table *t = heap_alloc(sizeof(*t));
	t->size = sizeof(*t)/sizeof(long);
	t->database_name = db;
	t->name = tablename;

	struct closure cl = {2, t, {2, 3}};
	struct lookUp lu = { RowMatcher, &cl};
	if(tree_findSingle(tn, &lu)){
		fprintf(stderr, "Error: Table %s already exists in database %s\n", tablename, db);
		exit(1);
	}
	tn = tree_insert(tn, t, &lu);
	return storeIndex("tables_index", tn, tni);
}
TreeNode createNewColumn(const char *db, const char *tablename, const char *columnname,
	const char *type, int isprimarykey, int auto_increment, int notnullable, TreeNode tni){
	if (!db){
		return tni;
	}
	TreeNode tn = findIndex("columns_index", tni);
	struct column *c = heap_alloc(sizeof(*c));
	c->size = sizeof(*c)/sizeof(long);
	c->database_name = db;
	c->table_name = tablename;
	c->name = columnname;
	c->type = type;
	c->isprimarykey = fixnum(isprimarykey);
	c->auto_increment = fixnum(auto_increment);
	c->notnullable = fixnum(notnullable);

	struct closure cl = {3, c, {2, 3, 4}};
	struct lookUp lu = { RowMatcher, &cl};
	if(tree_findSingle(tn, &lu)){
		fprintf(stderr, "Error: Column %s already exists in database %s.%s\n",
				columnname, db, tablename);
		exit(1);
	}
	tn = tree_insert(tn, c, &lu);
	return storeIndex("columns_index", tn, tni);
}
