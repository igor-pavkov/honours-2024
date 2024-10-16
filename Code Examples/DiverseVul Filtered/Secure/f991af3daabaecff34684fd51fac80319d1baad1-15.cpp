static int msg_insert(struct msg_msg *msg, struct mqueue_inode_info *info)
{
	struct rb_node **p, *parent = NULL;
	struct posix_msg_tree_node *leaf;

	p = &info->msg_tree.rb_node;
	while (*p) {
		parent = *p;
		leaf = rb_entry(parent, struct posix_msg_tree_node, rb_node);

		if (likely(leaf->priority == msg->m_type))
			goto insert_msg;
		else if (msg->m_type < leaf->priority)
			p = &(*p)->rb_left;
		else
			p = &(*p)->rb_right;
	}
	if (info->node_cache) {
		leaf = info->node_cache;
		info->node_cache = NULL;
	} else {
		leaf = kmalloc(sizeof(*leaf), GFP_ATOMIC);
		if (!leaf)
			return -ENOMEM;
		INIT_LIST_HEAD(&leaf->msg_list);
	}
	leaf->priority = msg->m_type;
	rb_link_node(&leaf->rb_node, parent, p);
	rb_insert_color(&leaf->rb_node, &info->msg_tree);
insert_msg:
	info->attr.mq_curmsgs++;
	info->qsize += msg->m_ts;
	list_add_tail(&msg->m_list, &leaf->msg_list);
	return 0;
}