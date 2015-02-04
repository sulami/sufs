struct node {
	int key;
	int data;
	struct node *left;
	struct node *right;
};

static struct node *_new_node(int, int);

static struct node *_parent(struct node *, int);

static int _left_path(struct node *);

static int _right_path(struct node *);

static struct node *_remove_node(struct node *);

struct node *btree_insert(struct node *, int, int);

int btree_lookup(struct node *, int);

struct node *btree_remove(struct node *, int);

int btree_size(struct node *);

int btree_depth(struct node *);

