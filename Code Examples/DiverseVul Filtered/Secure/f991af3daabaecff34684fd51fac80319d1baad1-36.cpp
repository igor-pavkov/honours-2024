static inline struct msg_msg *msg_get(struct mqueue_inode_info *info)
{
	struct rb_node **p, *parent = NULL;
	struct posix_msg_tree_node *leaf;
	struct msg_msg *msg;

try_again:
	p = &info->msg_tree.rb_node;
	while (*p) {
		parent = *p;
		/*
		 * During insert, low priorities go to the left and high to the
		 * right.  On receive, we want the highest priorities first, so
		 * walk all the way to the right.
		 */
		p = &(*p)->rb_right;
	}
	if (!parent) {
		if (info->attr.mq_curmsgs) {
			pr_warn_once("Inconsistency in POSIX message queue, "
				     "no tree element, but supposedly messages "
				     "should exist!\n");
			info->attr.mq_curmsgs = 0;
		}
		return NULL;
	}
	leaf = rb_entry(parent, struct posix_msg_tree_node, rb_node);
	if (unlikely(list_empty(&leaf->msg_list))) {
		pr_warn_once("Inconsistency in POSIX message queue, "
			     "empty leaf node but we haven't implemented "
			     "lazy leaf delete!\n");
		rb_erase(&leaf->rb_node, &info->msg_tree);
		if (info->node_cache) {
			kfree(leaf);
		} else {
			info->node_cache = leaf;
		}
		goto try_again;
	} else {
		msg = list_first_entry(&leaf->msg_list,
				       struct msg_msg, m_list);
		list_del(&msg->m_list);
		if (list_empty(&leaf->msg_list)) {
			rb_erase(&leaf->rb_node, &info->msg_tree);
			if (info->node_cache) {
				kfree(leaf);
			} else {
				info->node_cache = leaf;
			}
		}
	}
	info->attr.mq_curmsgs--;
	info->qsize -= msg->m_ts;
	return msg;
}