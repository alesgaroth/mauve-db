#include <stdio.h>
#include <stdlib.h>
#include "../src/rbtree.h"
#include <strings.h>

/*
typedef int (*Matcher)(Row datum, Closure c);

struct lookUp{
	Matcher m;
	Closure c;
};

TreeNode tree_insert(TreeNode tn, Row newrow, struct lookUp*lu);
Row tree_findSingle(TreeNode tn, struct lookUp *lu);
*/
struct row {
	long size; 
	char *name;
	char *value;
};

int sign(int v){
	return (((v)==0)?0:(((v)<0)?-1:1));
}

int RowMatcher(Row r_, Closure c){
	struct row *r = (struct row*)r_;
	struct row *mr = (struct row*)c;
	return sign(strcasecmp(r->name, mr->name));
}
Row newRow(char *name, char *value){
	struct row *r= malloc(sizeof(*r));
	r->size = (3<<3)|5;
	r->name = name;
	r->value = value;
	return r;
}

int RowMatcher2(Row r_, Closure c){
	struct row *r = (struct row*)r_;
	char *key = (char *)c;
	return sign(strcasecmp(r->name, key));
}

#define report(err) reporter(__FILE__, __LINE__, err)

void reporter(const char *file, int line, const char *error){
	fprintf(stderr, "%s:%d: %s\n", file, line, error);
	exit(1);
}

struct testdata {
	char *key;
	char *value;
} testdata [] = {
	{ "one", "uno"},
	{ "two", "duo"},
	{ "three", "trio"},
	{ "four", "quatro"},
	{ "five", "cinco"},
	{ "six", "seiso"},
	{ "seven", "septo"},
	{ "eight", "octo"},
	{ "nine", "nono"},
	{ "ten", "deco"},
	{ "eleven", "decuno"},
	{ "twelve", "decduo"},
	{ "thirteen", "dectrio"},
	{ "fourteen", "decquatro"},
	{ "fifteen", "deccinco"},
	{ "sixteen", "decseiso"},
	{ "seventeen", "decsepto"},
	{ "eighteen", "decocto"},
	{ "nineteen", "decnono"},
	{ "twenty", "duodeco"}
};

TreeNode doinsert(TreeNode tn, char *key, char *value, char *file, int line){
	Row r = newRow(key, value);
	struct lookUp lu = { RowMatcher, r};
	tn = tree_insert(tn, r, &lu);
	if (!tn) reporter(file, line, "expected tree_insert;  returned NULL;");
	return tn;
}
void dolookup(TreeNode tn, char *key, char *value, char *file, int line){
	struct lookUp lu = (struct lookUp){ RowMatcher2, key};
	Row v = tree_findSingle(tn, &lu);
	if (!v) reporter(file, line, "looked up by key failed");
	if (strcasecmp(((struct row*)v)->value, value)) reporter(file, line, "looked up by key failed");
}

int main(int argc, char **argv){

	TreeNode tn = doinsert(NULL, "mydatabase", "bob", __FILE__, __LINE__);
	tn = doinsert(tn, "myseconddb", "john", __FILE__, __LINE__);

	dolookup(tn, "mydatabase", "bob", __FILE__, __LINE__);
	dolookup(tn, "myseconddb", "john", __FILE__, __LINE__);

	TreeNode tn2 = tn;
	for(int j; j < 20; j += 1){
		tn = doinsert(tn, testdata[j].key, testdata[j].value, __FILE__, __LINE__);
	}

	
	struct lookUp lu = (struct lookUp){ RowMatcher2, "four"};
	Row v = tree_findSingle(tn2, &lu);
	if (v) report("found four in the tree we should not have");

	dolookup(tn, "four", "quatro", __FILE__, __LINE__);

	for(int j; j < 20; j += 1){
		dolookup(tn, testdata[j].key, testdata[j].value, __FILE__, __LINE__);
	}

	dolookup(tn, "myseconddb", "john", __FILE__, __LINE__);

	return 0;
}
