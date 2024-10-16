static inline int search_dirblock(struct buffer_head *bh,
				  struct inode *dir,
				  const struct qstr *d_name,
				  unsigned int offset,
				  struct ext4_dir_entry_2 ** res_dir)
{
	struct ext4_dir_entry_2 * de;
	char * dlimit;
	int de_len;
	const char *name = d_name->name;
	int namelen = d_name->len;

	de = (struct ext4_dir_entry_2 *) bh->b_data;
	dlimit = bh->b_data + dir->i_sb->s_blocksize;
	while ((char *) de < dlimit) {
		/* this code is executed quadratically often */
		/* do minimal checking `by hand' */

		if ((char *) de + namelen <= dlimit &&
		    ext4_match (namelen, name, de)) {
			/* found a match - just to be sure, do a full check */
			if (!ext4_check_dir_entry("ext4_find_entry",
						  dir, de, bh, offset))
				return -1;
			*res_dir = de;
			return 1;
		}
		/* prevent looping on a bad block */
		de_len = ext4_rec_len_from_disk(de->rec_len);
		if (de_len <= 0)
			return -1;
		offset += de_len;
		de = (struct ext4_dir_entry_2 *) ((char *) de + de_len);
	}
	return 0;
}