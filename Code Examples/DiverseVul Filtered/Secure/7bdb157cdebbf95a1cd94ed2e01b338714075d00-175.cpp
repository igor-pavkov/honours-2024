static bool perf_addr_filter_match(struct perf_addr_filter *filter,
				     struct file *file, unsigned long offset,
				     unsigned long size)
{
	/* d_inode(NULL) won't be equal to any mapped user-space file */
	if (!filter->path.dentry)
		return false;

	if (d_inode(filter->path.dentry) != file_inode(file))
		return false;

	if (filter->offset > offset + size)
		return false;

	if (filter->offset + filter->size < offset)
		return false;

	return true;
}