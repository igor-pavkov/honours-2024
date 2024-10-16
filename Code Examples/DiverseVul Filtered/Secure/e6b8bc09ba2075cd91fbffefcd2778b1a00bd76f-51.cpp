static int ext4_delete_entry(handle_t *handle,
			     struct inode *dir,
			     struct ext4_dir_entry_2 *de_del,
			     struct buffer_head *bh)
{
	struct ext4_dir_entry_2 *de, *pde;
	int i;

	i = 0;
	pde = NULL;
	de = (struct ext4_dir_entry_2 *) bh->b_data;
	while (i < bh->b_size) {
		if (!ext4_check_dir_entry("ext4_delete_entry", dir, de, bh, i))
			return -EIO;
		if (de == de_del)  {
			BUFFER_TRACE(bh, "get_write_access");
			ext4_journal_get_write_access(handle, bh);
			if (pde)
				pde->rec_len = ext4_rec_len_to_disk(
					ext4_rec_len_from_disk(pde->rec_len) +
					ext4_rec_len_from_disk(de->rec_len));
			else
				de->inode = 0;
			dir->i_version++;
			BUFFER_TRACE(bh, "call ext4_handle_dirty_metadata");
			ext4_handle_dirty_metadata(handle, dir, bh);
			return 0;
		}
		i += ext4_rec_len_from_disk(de->rec_len);
		pde = de;
		de = ext4_next_entry(de);
	}
	return -ENOENT;
}