static int ocfs2_file_clone_range(struct file *file_in,
				  loff_t pos_in,
				  struct file *file_out,
				  loff_t pos_out,
				  u64 len)
{
	return ocfs2_reflink_remap_range(file_in, pos_in, file_out, pos_out,
					 len, false);
}