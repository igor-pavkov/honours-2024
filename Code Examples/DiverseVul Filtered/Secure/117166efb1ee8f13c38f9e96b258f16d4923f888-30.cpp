static int ext4_add_dirent_to_inline(handle_t *handle,
				     struct ext4_filename *fname,
				     struct inode *dir,
				     struct inode *inode,
				     struct ext4_iloc *iloc,
				     void *inline_start, int inline_size)
{
	int		err;
	struct ext4_dir_entry_2 *de;

	err = ext4_find_dest_de(dir, inode, iloc->bh, inline_start,
				inline_size, fname, &de);
	if (err)
		return err;

	BUFFER_TRACE(iloc->bh, "get_write_access");
	err = ext4_journal_get_write_access(handle, iloc->bh);
	if (err)
		return err;
	ext4_insert_dentry(inode, de, inline_size, fname);

	ext4_show_inline_dir(dir, iloc->bh, inline_start, inline_size);

	/*
	 * XXX shouldn't update any times until successful
	 * completion of syscall, but too many callers depend
	 * on this.
	 *
	 * XXX similarly, too many callers depend on
	 * ext4_new_inode() setting the times, but error
	 * recovery deletes the inode, so the worst that can
	 * happen is that the times are slightly out of date
	 * and/or different from the directory change time.
	 */
	dir->i_mtime = dir->i_ctime = current_time(dir);
	ext4_update_dx_flag(dir);
	inode_inc_iversion(dir);
	return 1;
}