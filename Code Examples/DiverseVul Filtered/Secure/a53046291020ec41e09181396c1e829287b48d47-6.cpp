static int jfs_write_begin(struct file *file, struct address_space *mapping,
				loff_t pos, unsigned len, unsigned flags,
				struct page **pagep, void **fsdata)
{
	int ret;

	ret = nobh_write_begin(mapping, pos, len, flags, pagep, fsdata,
				jfs_get_block);
	if (unlikely(ret))
		jfs_write_failed(mapping, pos + len);

	return ret;
}