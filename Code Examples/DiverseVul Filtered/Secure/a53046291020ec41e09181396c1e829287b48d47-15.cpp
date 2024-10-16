struct inode *jfs_iget(struct super_block *sb, unsigned long ino)
{
	struct inode *inode;
	int ret;

	inode = iget_locked(sb, ino);
	if (!inode)
		return ERR_PTR(-ENOMEM);
	if (!(inode->i_state & I_NEW))
		return inode;

	ret = diRead(inode);
	if (ret < 0) {
		iget_failed(inode);
		return ERR_PTR(ret);
	}

	if (S_ISREG(inode->i_mode)) {
		inode->i_op = &jfs_file_inode_operations;
		inode->i_fop = &jfs_file_operations;
		inode->i_mapping->a_ops = &jfs_aops;
	} else if (S_ISDIR(inode->i_mode)) {
		inode->i_op = &jfs_dir_inode_operations;
		inode->i_fop = &jfs_dir_operations;
	} else if (S_ISLNK(inode->i_mode)) {
		if (inode->i_size >= IDATASIZE) {
			inode->i_op = &page_symlink_inode_operations;
			inode_nohighmem(inode);
			inode->i_mapping->a_ops = &jfs_aops;
		} else {
			inode->i_op = &jfs_fast_symlink_inode_operations;
			inode->i_link = JFS_IP(inode)->i_inline;
			/*
			 * The inline data should be null-terminated, but
			 * don't let on-disk corruption crash the kernel
			 */
			inode->i_link[inode->i_size] = '\0';
		}
	} else {
		inode->i_op = &jfs_file_inode_operations;
		init_special_inode(inode, inode->i_mode, inode->i_rdev);
	}
	unlock_new_inode(inode);
	return inode;
}