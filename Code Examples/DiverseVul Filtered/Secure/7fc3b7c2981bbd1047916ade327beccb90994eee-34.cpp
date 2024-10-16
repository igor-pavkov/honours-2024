static void udf_do_extend_final_block(struct inode *inode,
				      struct extent_position *last_pos,
				      struct kernel_long_ad *last_ext,
				      uint32_t final_block_len)
{
	struct super_block *sb = inode->i_sb;
	uint32_t added_bytes;

	added_bytes = final_block_len -
		      (last_ext->extLength & (sb->s_blocksize - 1));
	last_ext->extLength += added_bytes;
	UDF_I(inode)->i_lenExtents += added_bytes;

	udf_write_aext(inode, last_pos, &last_ext->extLocation,
			last_ext->extLength, 1);
}