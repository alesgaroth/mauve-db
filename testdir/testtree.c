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

int main(int argc, char **argv){
	Row r = newRow("mydatabase", "bob");
	struct lookUp lu = { RowMatcher, r};
	TreeNode tn = tree_insert(NULL, r, &lu);
	if (!tn) report("expected tree_insert;  returned NULL;");
	r = newRow("myseconddb", "john");
	lu = (struct lookUp){ RowMatcher, r};
	tn = tree_insert(tn, r, &lu);
	if (!tn) report("expected tree_insert(2);  returned NULL;");
	lu = (struct lookUp){ RowMatcher2, "mydatabase"};
	Row v = tree_findSingle(tn, &lu);
	if (strcasecmp(((struct row*)v)->value, "bob")) report("looked up by key failed 1");
	
	lu = (struct lookUp){ RowMatcher2, "myseconddb"};
	v = tree_findSingle(tn, &lu);
	if (!v) report("looked up by key failed 2");
	if (strcasecmp(((struct row*)v)->value, "john")) report("looked up by key failed 3");
	TreeNode tn2 = tn;
	for(int j; j < 20; j += 1){
		r = newRow(testdata[j].key, testdata[j].value);
		lu = (struct lookUp){RowMatcher, r};
		tn = tree_insert(tn, r, &lu);
		if (!tn) report("insert failed j");
	}
	return 0;
}
