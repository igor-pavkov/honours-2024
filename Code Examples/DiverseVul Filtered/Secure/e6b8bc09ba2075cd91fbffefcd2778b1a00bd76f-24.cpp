static int empty_dir(struct inode *inode)
{
	unsigned int offset;
	struct buffer_head *bh;
	struct ext4_dir_entry_2 *de, *de1;
	struct super_block *sb;
	int err = 0;

	sb = inode->i_sb;
	if (inode->i_size < EXT4_DIR_REC_LEN(1) + EXT4_DIR_REC_LEN(2) ||
	    !(bh = ext4_bread(NULL, inode, 0, 0, &err))) {
		if (err)
			ext4_error(inode->i_sb, __func__,
				   "error %d reading directory #%lu offset 0",
				   err, inode->i_ino);
		else
			ext4_warning(inode->i_sb, __func__,
				     "bad directory (dir #%lu) - no data block",
				     inode->i_ino);
		return 1;
	}
	de = (struct ext4_dir_entry_2 *) bh->b_data;
	de1 = ext4_next_entry(de);
	if (le32_to_cpu(de->inode) != inode->i_ino ||
			!le32_to_cpu(de1->inode) ||
			strcmp(".", de->name) ||
			strcmp("..", de1->name)) {
		ext4_warning(inode->i_sb, "empty_dir",
			     "bad directory (dir #%lu) - no `.' or `..'",
			     inode->i_ino);
		brelse(bh);
		return 1;
	}
	offset = ext4_rec_len_from_disk(de->rec_len) +
		 ext4_rec_len_from_disk(de1->rec_len);
	de = ext4_next_entry(de1);
	while (offset < inode->i_size) {
		if (!bh ||
			(void *) de >= (void *) (bh->b_data+sb->s_blocksize)) {
			err = 0;
			brelse(bh);
			bh = ext4_bread(NULL, inode,
				offset >> EXT4_BLOCK_SIZE_BITS(sb), 0, &err);
			if (!bh) {
				if (err)
					ext4_error(sb, __func__,
						   "error %d reading directory"
						   " #%lu offset %u",
						   err, inode->i_ino, offset);
				offset += sb->s_blocksize;
				continue;
			}
			de = (struct ext4_dir_entry_2 *) bh->b_data;
		}
		if (!ext4_check_dir_entry("empty_dir", inode, de, bh, offset)) {
			de = (struct ext4_dir_entry_2 *)(bh->b_data +
							 sb->s_blocksize);
			offset = (offset | (sb->s_blocksize - 1)) + 1;
			continue;
		}
		if (le32_to_cpu(de->inode)) {
			brelse(bh);
			return 0;
		}
		offset += ext4_rec_len_from_disk(de->rec_len);
		de = ext4_next_entry(de);
	}
	brelse(bh);
	return 1;
}