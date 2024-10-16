static int ext4_dquot_drop(struct inode *inode)
{
	handle_t *handle;
	int ret, err;

	/* We may delete quota structure so we need to reserve enough blocks */
	handle = ext4_journal_start(inode, 2*EXT4_QUOTA_DEL_BLOCKS(inode->i_sb));
	if (IS_ERR(handle)) {
		/*
		 * We call dquot_drop() anyway to at least release references
		 * to quota structures so that umount does not hang.
		 */
		dquot_drop(inode);
		return PTR_ERR(handle);
	}
	ret = dquot_drop(inode);
	err = ext4_journal_stop(handle);
	if (!ret)
		ret = err;
	return ret;
}