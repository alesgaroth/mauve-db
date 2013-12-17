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
		assert(color == Black || "a leaf node is not black");
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

static TreeNode blacken(TreeNode tn){
	return ((char*)treenode(tn))+Black;
}
#define leftof(tn) (treenode(tn)->left)
#define rightof(tn) (treenode(tn)->right)
#define rowof(tn) ((Row)(istreenode(tn)?treenode(tn)->datum:tn))

static TreeNode rebalance(Row r, TreeNode left, TreeNode right, enum colors color){
	if (color == Black){
		if (isred(left)&& isred(leftof(left))){
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

void printN(int n){
	for(; n >= 0; n -= 1){
		putchar(' ');
	}
}

void printTree2(TreeNode tn, int depth){
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
	printTree2(t->left, depth+1);
	printN(depth);
	printf("%p :  %p\n", tn, t->datum); 
	printTree2(t->right, depth+1);
}

void printTree(TreeNode tn){
	printTree2(tn, 0);
}
