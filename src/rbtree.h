
enum colors {  NoColor, CharString, Red, Black, List, Integer, Reserved,  Enum};
typedef void *TreeNode;
typedef void *Row;
typedef void *Closure;

/**
* returns 0 if matches.
* returns 1 if the row is less
* returns -1 if the row is more 
*/
typedef int (*Matcher)(Row datum, Closure c);

struct lookUp{
	Matcher m;
	Closure c;
};

/* inserts a "row" into the tree
 * using the given matcher to order */
TreeNode tree_insert(TreeNode tn, Row newrow, struct lookUp*lu);


/* call matcher on the way down the tree. */
void tree_lookup(TreeNode tn, struct lookUp *);

/* find the first row that matches */
Row tree_findSingle(TreeNode tn, struct lookUp *lu);
