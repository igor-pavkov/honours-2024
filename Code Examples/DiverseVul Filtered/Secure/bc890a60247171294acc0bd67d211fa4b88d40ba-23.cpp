static int ext4_ext_truncate_extend_restart(handle_t *handle,
					    struct inode *inode,
					    int needed)
{
	int err;

	if (!ext4_handle_valid(handle))
		return 0;
	if (handle->h_buffer_credits >= needed)
		return 0;
	/*
	 * If we need to extend the journal get a few extra blocks
	 * while we're at it for efficiency's sake.
	 */
	needed += 3;
	err = ext4_journal_extend(handle, needed - handle->h_buffer_credits);
	if (err <= 0)
		return err;
	err = ext4_truncate_restart_trans(handle, inode, needed);
	if (err == 0)
		err = -EAGAIN;

	return err;
}