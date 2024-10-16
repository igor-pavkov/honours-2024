static struct rds_mr *rds_mr_tree_walk(struct rb_root *root, u64 key,
				       struct rds_mr *insert)
{
	struct rb_node **p = &root->rb_node;
	struct rb_node *parent = NULL;
	struct rds_mr *mr;

	while (*p) {
		parent = *p;
		mr = rb_entry(parent, struct rds_mr, r_rb_node);

		if (key < mr->r_key)
			p = &(*p)->rb_left;
		else if (key > mr->r_key)
			p = &(*p)->rb_right;
		else
			return mr;
	}

	if (insert) {
		rb_link_node(&insert->r_rb_node, parent, p);
		rb_insert_color(&insert->r_rb_node, root);
		refcount_inc(&insert->r_refcount);
	}
	return NULL;
}