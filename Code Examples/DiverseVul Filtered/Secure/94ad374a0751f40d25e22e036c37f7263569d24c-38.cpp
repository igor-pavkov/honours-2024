int sync_page_range_nolock(struct inode *inode, struct address_space *mapping,
			   loff_t pos, loff_t count)
{
	pgoff_t start = pos >> PAGE_CACHE_SHIFT;
	pgoff_t end = (pos + count - 1) >> PAGE_CACHE_SHIFT;
	int ret;

	if (!mapping_cap_writeback_dirty(mapping) || !count)
		return 0;
	ret = filemap_fdatawrite_range(mapping, pos, pos + count - 1);
	if (ret == 0)
		ret = generic_osync_inode(inode, mapping, OSYNC_METADATA);
	if (ret == 0)
		ret = wait_on_page_writeback_range(mapping, start, end);
	return ret;
}