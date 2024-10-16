static int ext4_ext_zeroout(struct inode *inode, struct ext4_extent *ex)
{
	ext4_fsblk_t ee_pblock;
	unsigned int ee_len;

	ee_len    = ext4_ext_get_actual_len(ex);
	ee_pblock = ext4_ext_pblock(ex);
	return ext4_issue_zeroout(inode, le32_to_cpu(ex->ee_block), ee_pblock,
				  ee_len);
}