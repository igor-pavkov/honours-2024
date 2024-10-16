static void delayed_mntput(struct work_struct *unused)
{
	struct llist_node *node = llist_del_all(&delayed_mntput_list);
	struct llist_node *next;

	for (; node; node = next) {
		next = llist_next(node);
		cleanup_mnt(llist_entry(node, struct mount, mnt_llist));
	}
}