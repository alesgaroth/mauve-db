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
int report2(yyscan_t scandata, const char *note, int num);
const char *string_dup(const char *val);

TreeNode createNewRow(const char *db, const char *tablename, int numcols,
	TreeNode colnames, ListNode colvals, ListNode cols, TreeNode tn);
int parseSQL(void);

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
	return parseSQL();
}
int parseSQL(void){
	struct yydata data = {NULL};
	yyscan_t scandata;
	if(yylex_init_extra(&data,&scandata)){
		perror("yylex_init_extra:");
		return 3;
	}
	TreeNode tn = NULL;
	tn = createIndexes();
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
		case INSERT:
			switch(yylex(scandata)){
			case YY_NULL:
			case INTO:
				switch(yylex(scandata)){
				case YY_NULL:
				case NAME:{
					const char *tablename = string_dup(data.strval);
					const char *db = current_db;
					int colnamesdone = 0;
					int numcols = 0;
					TreeNode colnames = NULL;
					ListNode cols = NULL;
					int colsdone = 0;
					ListNode colvals = NULL;
					parseval = 0;
					switch(yylex(scandata)){
					case YY_NULL:return report(scandata, "unexpected end of file");
					case '.':
						db = tablename;
						switch(yylex(scandata)){
						case YY_NULL:return report(scandata, "unexpected end of file");
						case NAME:
							tablename = string_dup(data.strval);
							break;
						default: return report(scandata, "Unexpected token:");
						}
						parseval = yylex(scandata);
						if (parseval == '(')
					case '(':
						{
						do {
							struct lookUp lu = { (Matcher)strcmp, NULL};
							switch(yylex(scandata)){
							case YY_NULL:return report(scandata, "unexpected end of file");
							case NAME:
								lu.c = string_dup(data.strval);
								colnames = tree_insert(colnames, lu.c, &lu);
								cols = newList(lu.c, cols);
								numcols += 1;
								switch(yylex(scandata)){
								case YY_NULL:return report(scandata, "unexpected end of file");
								case ')':colnamesdone = 1;/*fall through */
								case ',':
									break;
								default: return report(scandata, "Unexpected token:");
								}
								break;
							default: return report(scandata, "Unexpected token:");
							}
						} while(!colnamesdone);
						switch(yylex(scandata)){
						case YY_NULL:return report(scandata, "unexpected end of file");
						case VALUES:
							do {
								int val;
								switch(yylex(scandata)){
								case YY_NULL:return report(scandata, "unexpected end of file");
								case '(':
									switch(val = yylex(scandata)){
									case YY_NULL:return report(scandata, "unexpected end of file");
									case NUMBER:  colvals = newList((Row)(long)fixnum(atoi(data.strval)), colvals);
										break;
									case STRING:
										colvals = newList(string_dup(data.strval), colvals);
										break;
									default: return report2(scandata, "UneXpected token:", val);
									}
									switch(yylex(scandata)){
									case YY_NULL:return report(scandata, "unexpected end of file");
									case ')': colsdone = 1;
									case ',': break;
									default: return report(scandata, "Unexpected token:");
									}
									break;
								default: return report(scandata, "Unexpected token:");
								}
							} while(!colsdone);
							tn = createNewRow(db, tablename, numcols, colnames, colvals, cols, tn);
							break;
						default: return report(scandata, "Unexpected token:");
						}

						break;
						} if (parseval == SET){
					case SET:
						colsdone = 0;
						do {
							struct lookUp lu = { (Matcher)strcmp, NULL};
							switch(yylex(scandata)){
							case YY_NULL:return report(scandata, "unexpected end of file");
							case NAME:
								numcols += 1;
								lu.c = string_dup(data.strval);
								colnames = tree_insert(colnames, lu.c, &lu);
								cols = newList(lu.c, cols);
								switch(yylex(scandata)){
								case YY_NULL:return report(scandata, "unexpected end of file");
								case '=': break;
								default: return report(scandata, "Unexpected token:");
								}
								switch(yylex(scandata)){
								case YY_NULL:return report(scandata, "unexpected end of file");
								case STRING:
									colvals = newList(string_dup(data.strval), colvals);
									break;
								case NUMBER:  colvals = newList((Row)(long)fixnum(atoi(data.strval)), colvals);
									break;
								default: return report(scandata, "Unexpected token:");
								}
								switch(yylex(scandata)){
								case YY_NULL:return report(scandata, "unexpected end of file");
								case ';': colsdone = 1;
								case ',': break;
								default: return report(scandata, "Unexpected token:");
								}
								break;
							case ';': colsdone = 1;
							default: return report(scandata, "Unexpected token:");
							}
						} while(!colsdone);
						tn = createNewRow(db, tablename, numcols, colnames, colvals, cols, tn);
						break;
						}
					default: return report(scandata, "Unexpected token:");
					}
					break;
					}
				default: return report(scandata, "Unexpected token:");
				}
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
	yylex_destroy(scandata);
	scandata = NULL;
	return 0;
}

int report2(yyscan_t scandata, const char *note, int num){
	fprintf(stderr, "%s:%d:%d parser error %s (%d)\"%s\"\n",
		"stdin", yyget_lineno(scandata), yyget_column(scandata),
		note, num, yyget_text(scandata));
	return 1;
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
	const struct index*ip =  tree_findSingle(tn, &lu);
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


int binsearch(const void *name, Row *pp){
	int numitems = defix(pp[0]) - 2;
	pp = pp + 2;
	int k = 0;
	for(int j = numitems/2; j <numitems; j = (numitems+k)/2){
		int m = sign(strcmp(pp[j], name));
		switch(m){
		case 0: return j + 2;
		case -1: k  = j; break;
		case 1: numitems = j; break;
		}
		if (k > numitems -1){
			break;
		}
	}
	if (!strcmp(pp[k], name)) return k;
	if (!strcmp(pp[numitems], name)) return k;
	return 1;
}

TreeNode createNewRow(const char *db, const char *tablename, int numcols,
		TreeNode colnames, ListNode colvals, ListNode cols, TreeNode tn){
	Row *p = heap_alloc((2+numcols) *sizeof(p));
	Row *pp = heap_alloc((2+numcols) *sizeof(p));
	p[0] = pp;
	pp[0] = (Row)(long)fixnum(2+numcols);
	int k;
	const void *name, *val;
	TreeIterator it = tree_iterator(colnames);
	for(k=2; (name = treei_value(it)); k+= 1){
		pp[k] = name;
		it = treei_next(it);
	}
	for(k = 2; cols && colvals; k += 1){
		name = list_first(cols);
		val = list_first(colvals);
		cols = list_next(cols);
		colvals = list_next(colvals);
		p[binsearch(name, (Row*)p[0])] = val;
	}
	/* okay p is the row.*/
	/* now for each index on the given table,
		insert the row into it */
	return tn;
}
