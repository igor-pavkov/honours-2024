static ssize_t ocfs2_file_dedupe_range(struct file *src_file,
				       u64 loff,
				       u64 len,
				       struct file *dst_file,
				       u64 dst_loff)
{
	int error;

	error = ocfs2_reflink_remap_range(src_file, loff, dst_file, dst_loff,
					  len, true);
	if (error)
		return error;
	return len;
}