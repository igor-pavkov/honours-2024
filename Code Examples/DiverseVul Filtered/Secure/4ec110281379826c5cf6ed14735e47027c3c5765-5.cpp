static int ext4_dquot_initialize(struct inode *inode, int type)
{
	handle_t *handle;
	int ret, err;

	/* We may create quota structure so we need to reserve enough blocks */
	handle = ext4_journal_start(inode, 2*EXT4_QUOTA_INIT_BLOCKS(inode->i_sb));
	if (IS_ERR(handle))
		return PTR_ERR(handle);
	ret = dquot_initialize(inode, type);
	err = ext4_journal_stop(handle);
	if (!ret)
		ret = err;
	return ret;
}