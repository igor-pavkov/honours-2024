int ocfs2_zero_extend(struct inode *inode, struct buffer_head *di_bh,
		      loff_t zero_to_size)
{
	int ret = 0;
	u64 zero_start, range_start = 0, range_end = 0;
	struct super_block *sb = inode->i_sb;

	zero_start = ocfs2_align_bytes_to_blocks(sb, i_size_read(inode));
	trace_ocfs2_zero_extend((unsigned long long)OCFS2_I(inode)->ip_blkno,
				(unsigned long long)zero_start,
				(unsigned long long)i_size_read(inode));
	while (zero_start < zero_to_size) {
		ret = ocfs2_zero_extend_get_range(inode, di_bh, zero_start,
						  zero_to_size,
						  &range_start,
						  &range_end);
		if (ret) {
			mlog_errno(ret);
			break;
		}
		if (!range_end)
			break;
		/* Trim the ends */
		if (range_start < zero_start)
			range_start = zero_start;
		if (range_end > zero_to_size)
			range_end = zero_to_size;

		ret = ocfs2_zero_extend_range(inode, range_start,
					      range_end, di_bh);
		if (ret) {
			mlog_errno(ret);
			break;
		}
		zero_start = range_end;
	}

	return ret;
}