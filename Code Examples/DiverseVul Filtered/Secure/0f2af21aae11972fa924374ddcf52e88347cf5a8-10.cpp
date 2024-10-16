static int ext4_valid_extent(struct inode *inode, struct ext4_extent *ext)
{
	ext4_fsblk_t block = ext4_ext_pblock(ext);
	int len = ext4_ext_get_actual_len(ext);
	ext4_lblk_t lblock = le32_to_cpu(ext->ee_block);
	ext4_lblk_t last = lblock + len - 1;

	if (lblock > last)
		return 0;
	return ext4_data_block_valid(EXT4_SB(inode->i_sb), block, len);
}