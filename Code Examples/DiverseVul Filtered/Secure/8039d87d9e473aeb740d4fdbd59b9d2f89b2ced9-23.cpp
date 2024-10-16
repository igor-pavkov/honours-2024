static int check_ref(struct btrfs_trans_handle *trans, struct btrfs_root *root,
		     u64 disko)
{
	struct seq_list tree_mod_seq_elem = SEQ_LIST_INIT(tree_mod_seq_elem);
	struct ulist *roots;
	struct ulist_iterator uiter;
	struct ulist_node *root_node = NULL;
	int ret;

	if (!root->fs_info->quota_enabled)
		return 1;

	btrfs_get_tree_mod_seq(root->fs_info, &tree_mod_seq_elem);
	ret = btrfs_find_all_roots(trans, root->fs_info, disko,
				   tree_mod_seq_elem.seq, &roots);
	if (ret < 0)
		goto out;
	ret = 0;
	ULIST_ITER_INIT(&uiter);
	while ((root_node = ulist_next(roots, &uiter))) {
		if (root_node->val == root->objectid) {
			ret = 1;
			break;
		}
	}
	ulist_free(roots);
out:
	btrfs_put_tree_mod_seq(root->fs_info, &tree_mod_seq_elem);
	return ret;
}