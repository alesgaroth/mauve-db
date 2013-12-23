
enum colors {  NoColor, CharString, Red, Black, List, Integer, Reserved,  Enum};
typedef const void *TreeNode;
typedef const void *Row;
typedef const void *Closure;
typedef const void *ListNode;
typedef const void *TreeIterator;

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

/* update the row, the lookup for the new row and the replaced row must be the same */
TreeNode tree_update(TreeNode tn, Row newrow, struct lookUp*lu);

/* delete the row */
TreeNode tree_deleteSingle(TreeNode tn, struct lookUp *);

/* find the first row that matches */
Row tree_findSingle(TreeNode tn, struct lookUp *lu);

#define fixnum(x) (((x)<<3)+Integer)
#define defix(x) (((long)(x))>>3)


TreeIterator treei_next(TreeIterator ti);
Row treei_value(TreeIterator it);
TreeIterator tree_iterator(TreeNode tn);


Row list_first(ListNode);
ListNode list_next(ListNode);
ListNode newList(Row left, ListNode right);
