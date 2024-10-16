static int clone_finish_inode_update(struct btrfs_trans_handle *trans,
				     struct inode *inode,
				     u64 endoff,
				     const u64 destoff,
				     const u64 olen,
				     int no_time_update)
{
	struct btrfs_root *root = BTRFS_I(inode)->root;
	int ret;

	inode_inc_iversion(inode);
	if (!no_time_update)
		inode->i_mtime = inode->i_ctime = CURRENT_TIME;
	/*
	 * We round up to the block size at eof when determining which
	 * extents to clone above, but shouldn't round up the file size.
	 */
	if (endoff > destoff + olen)
		endoff = destoff + olen;
	if (endoff > inode->i_size)
		btrfs_i_size_write(inode, endoff);

	ret = btrfs_update_inode(trans, root, inode);
	if (ret) {
		btrfs_abort_transaction(trans, root, ret);
		btrfs_end_transaction(trans, root);
		goto out;
	}
	ret = btrfs_end_transaction(trans, root);
out:
	return ret;
}