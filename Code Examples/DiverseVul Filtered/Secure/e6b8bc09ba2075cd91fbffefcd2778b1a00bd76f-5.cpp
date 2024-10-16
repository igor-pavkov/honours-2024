struct dentry *ext4_get_parent(struct dentry *child)
{
	__u32 ino;
	struct inode *inode;
	static const struct qstr dotdot = {
		.name = "..",
		.len = 2,
	};
	struct ext4_dir_entry_2 * de;
	struct buffer_head *bh;

	bh = ext4_find_entry(child->d_inode, &dotdot, &de);
	inode = NULL;
	if (!bh)
		return ERR_PTR(-ENOENT);
	ino = le32_to_cpu(de->inode);
	brelse(bh);

	if (!ext4_valid_inum(child->d_inode->i_sb, ino)) {
		ext4_error(child->d_inode->i_sb, "ext4_get_parent",
			   "bad inode number: %u", ino);
		return ERR_PTR(-EIO);
	}

	return d_obtain_alias(ext4_iget(child->d_inode->i_sb, ino));
}