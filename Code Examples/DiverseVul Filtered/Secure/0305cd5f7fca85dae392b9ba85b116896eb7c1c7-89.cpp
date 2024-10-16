struct inode *btrfs_iget(struct super_block *s, struct btrfs_key *location,
			 struct btrfs_root *root, int *new)
{
	struct inode *inode;

	inode = btrfs_iget_locked(s, location, root);
	if (!inode)
		return ERR_PTR(-ENOMEM);

	if (inode->i_state & I_NEW) {
		btrfs_read_locked_inode(inode);
		if (!is_bad_inode(inode)) {
			inode_tree_add(inode);
			unlock_new_inode(inode);
			if (new)
				*new = 1;
		} else {
			unlock_new_inode(inode);
			iput(inode);
			inode = ERR_PTR(-ESTALE);
		}
	}

	return inode;
}