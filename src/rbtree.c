#include "memman.h"
#include <assert.h>
#include "rbtree.h"
#include <stdio.h>


struct treeNode {
	Row datum;
	TreeNode left;
	TreeNode right;
};


static TreeNode newTreeNode(Row datum, TreeNode left, TreeNode right, enum colors color){
	if (0 && left == NULL && right == NULL){
		assert(color == Black && "a leaf node is not black");
		if (color == Black)
			return datum;
	}
	struct treeNode *tn = heap_alloc(sizeof(*tn));
	tn->left = left;
	tn->right = right;
	tn->datum = datum;
	return ((char*)tn)+color;
}

#define treenode(tn) ((struct treeNode*)(((size_t)tn)&-8))
#define gettag(tn) (((size_t)tn)&7)
#define isred(tn) (gettag(tn)==Red)
#define isblack(tn) (gettag(tn)==Black)
#define istreenode(tn) ((gettag(tn)&6) == Red)

static Row getRow(TreeNode tn){
	if (!tn) return NULL;
	switch(((size_t)tn)&7){
	case NoColor:
	case CharString:
	case Integer:
	case Enum:
	case List:
		return tn;
		break;
	case Red:
	case Black:
		return treenode(tn)->datum;
		break;
	default:
		return NULL;
	}
}

void tree_lookup(TreeNode tn, struct lookUp *lu){
	if (!tn) return;
	Row r = getRow(tn);
	if (!r)return;/*on a leaf */
	struct treeNode *t = treenode(tn);
	switch(lu->m(r, lu->c)){
	case 0: 
		tree_lookup(t->left, lu);
		tree_lookup(t->right, lu);
		break;
	case 1:
		tree_lookup(t->left, lu);
		break;
	case -1:
		tree_lookup(t->right, lu);
		break;
	}
}
TreeNode tree_update(TreeNode tn, Row newrow, struct lookUp*lu){
	Row r = getRow(tn);
	if (!r) return NULL; /* it's not here!*/
	struct treeNode *t = treenode(tn);
	switch(lu->m(r, lu->c)){
	case 0:
		return newTreeNode(newrow, t->left, t->right, gettag(tn));
	case 1:
		return newTreeNode(r, tree_update(t->left, newrow, lu), t->right, gettag(tn));
	case -1:
		return newTreeNode(r, t->left, tree_update(t->right, newrow, lu), gettag(tn));
	default:
		fprintf(stderr, "oops\n");
		exit(3);
	}
}
Row tree_findSingle(TreeNode tn, struct lookUp *lu){
	while(tn) {
		Row r = getRow(tn);
		if (!r)return NULL; /* leaf */
		struct treeNode *t = treenode(tn);
		switch(lu->m(r, lu->c)){
		case 0: 
			return r;
		case 1:
			tn = t->left;
			break;
		case -1:
			tn = t->right;
			break;
		}
	}
	return NULL;
}

struct insertData {
	Row newrow;
	struct lookUp *lu;
};

static TreeNode redden(TreeNode tn){
	return ((char*)treenode(tn))+Red;
}
static TreeNode blacken(TreeNode tn){
	return ((char*)treenode(tn))+Black;
}
#define leftof(tn) (treenode(tn)->left)
#define rightof(tn) (treenode(tn)->right)
#define rowof(tn) ((Row)(istreenode(tn)?treenode(tn)->datum:tn))

static TreeNode rebalance(Row r, TreeNode left, TreeNode right, enum colors color){
	if (color == Black){
		if (isred(left)&& isred(right)){
			return newTreeNode(r, blacken(left), blacken(right), Red);
		} else if (isred(left)&& isred(leftof(left))){
			return newTreeNode(rowof(left),
				blacken(leftof(left)),
				newTreeNode(r, rightof(left), right, Black), Red);
		} else if (isred(left) && isred(rightof(left))){
			return newTreeNode(rowof(rightof(left)),
				newTreeNode(rowof(left), leftof(left), leftof(rightof(left)), Black),
				newTreeNode(r, rightof(rightof(left)), right, Black), Red);
		}else if (isred(right) && isred(leftof(right))){
			return newTreeNode(rowof(leftof(right)),
				newTreeNode(r, left, leftof(leftof(right)), Black),
				newTreeNode(rowof(right), rightof(leftof(right)), rightof(right), Black), Red);
		} else if(isred(right) && isred(rightof(right))){
			return newTreeNode(rowof(right),
				newTreeNode(r, left, leftof(right), Black),
				blacken(rightof(right)), Red);
		}
	}
	return newTreeNode(r, left, right, color);
}

static TreeNode ins(TreeNode tn, struct insertData *id){
	if (tn == NULL)return newTreeNode(id->newrow, NULL, NULL, Red);
	Row r = getRow(tn);
	struct treeNode *t = treenode(tn);
	int color = isred(tn)?Red:Black;
	switch(id->lu->m(r, id->lu->c)){
	case 0: return newTreeNode(id->newrow, t->left, t->right, color);
	case 1: return rebalance(r, ins(t->left, id), t->right, color);
	case -1: return rebalance(r, t->left, ins(t->right, id), color);
	}
	assert(0 && "ins invalid code");
	return tn;
}

TreeNode tree_insert(TreeNode tn, Row newrow, struct lookUp*lu){
	//if (!tn) return newrow;
	struct insertData id = {newrow, lu};
	return blacken(ins(tn, &id));
}

static void printN(int n){
	for(; n >= 0; n -= 1){
		putchar(' ');
	}
}

typedef void (*PrintRow)(Row);
void printTree(TreeNode tn, PrintRow pr);

static void printTree2(TreeNode tn, int depth, PrintRow pr){
	if (!tn) {
		printN(depth);
		printf("NULL\n");
		return;
	}
	if (!istreenode(tn)) {
		printN(depth);
		printf("non tree node:%p\n", tn);
		return;
	}
	struct treeNode*t =treenode(tn);
	printTree2(t->left, depth+1, pr);
	printN(depth);
	printf("%p :", tn); 
	pr(t->datum);
	printf("\n");
	printTree2(t->right, depth+1, pr);
}

void printTree(TreeNode tn, PrintRow pr){
	printTree2(tn, 0, pr);
}

static TreeNode deleteRightmost(Row r, TreeNode left, TreeNode right, Row *rp){
	if (right){
		if (isred(right)){
			return rebalance(r, left, 
				deleteRightmost(rowof(right), leftof(right), rightof(right), rp), Red);
		} else if (isblack(left)){
			return rebalance(r, redden(left),	
				deleteRightmost(rowof(right), leftof(right), rightof(right), rp), Black);
		} else {
			return rebalance(rowof(left), leftof(left),
				deleteRightmost(r, rightof(left), right, rp), Red);
		}
	} else {
		*rp = r;
		return left;
	}
}
static TreeNode deleteLeftmost(Row r, TreeNode left, TreeNode right, Row *rp){
	if (left){
		if (isred(left)){
			return rebalance(r, 
				deleteLeftmost(rowof(left), leftof(left), rightof(left), rp),
				right, Red);
		} else if (isblack(right)){
			return rebalance(r, 
				deleteLeftmost(rowof(left), leftof(left), rightof(left), rp),
				redden(right), Black);
		} else {
			return rebalance(rowof(right),
				deleteLeftmost(r, leftof(right), left, rp), rightof(right), Red);
		}
	} else {
		*rp = r;
		return right;
	}
}

static TreeNode del(Row r, TreeNode left, TreeNode right, enum colors color, struct lookUp *lu){
	int m = lu->m(r, lu->c);
	switch(m){
	case 0:
		if (left != NULL){
			Row rightmostofleft = NULL;
			left = deleteRightmost(rowof(left), leftof(left), rightof(left), &rightmostofleft);
			assert(rightmostofleft != NULL && "rightmost returned null");
			if (left == NULL && right == NULL){
				return newTreeNode(rightmostofleft, NULL, NULL, Black);
			}
			return rebalance(rightmostofleft, left,
				redden(right), Black);
		}else if (right != NULL){
			Row leftmostofright = NULL;
			right = deleteLeftmost(rowof(right), leftof(right), rightof(right), &leftmostofright);
			assert(leftmostofright != NULL && "leftmost returned null");
			if (left == NULL && right == NULL){
				return newTreeNode(leftmostofright, NULL, NULL, Black);
			}
			return rebalance(leftmostofright, redden(left),
				right, Black);
		} else {
			return NULL;
		}
	case 1:
		if (!left){
			return newTreeNode(r, left, right, color);
		} else if (isred(left)){
			struct treeNode *l = treenode(left);
			return rebalance(r,
				del(l->datum, l->left, l->right, Red, lu),
				right, color);
		} else if (isblack(right)){
			struct treeNode *l = treenode(left);
			return rebalance(r,
				del(l->datum, l->left, l->right, Red, lu),
				redden(right), color);
		} else {
			struct treeNode*rt = treenode(right);
			return rebalance(rt->datum,
				del(r, left,rt->left, Red, lu),
				rt->right, color);
		}
	case -1:
		if (!right){
			return newTreeNode(r, left, right, color);
		}else if (isred(right)){
			struct treeNode*rt = treenode(right);
			return rebalance(r, left,
				del(rt->datum, rt->left, rt->right, Red, lu),
				color);
		} else if (isblack(left)){
			struct treeNode*rt = treenode(right);
			return rebalance(r, redden(left),
				del(rt->datum, rt->left, rt->right, Red, lu),
				color);
		} else {
			struct treeNode *l = treenode(left);
			return rebalance(l->datum, l->left,
				del(r, l->right, right, Red, lu),
				color);
		}
	}
	assert(0 &&  "unreachable code");
}

TreeNode tree_deleteSingle(TreeNode tn, struct lookUp *lu){
	if (tn == NULL) return NULL;
	if (!istreenode(tn)){
		return (lu->m(tn, lu->c))?NULL:tn;
	}
	struct treeNode*t =treenode(tn);
	return blacken(del(t->datum, t->left, t->right, Red, lu));
}
