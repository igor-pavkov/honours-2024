int pagecache_write_end(struct file *file, struct address_space *mapping,
				loff_t pos, unsigned len, unsigned copied,
				struct page *page, void *fsdata)
{
	const struct address_space_operations *aops = mapping->a_ops;
	int ret;

	if (aops->write_end) {
		mark_page_accessed(page);
		ret = aops->write_end(file, mapping, pos, len, copied,
							page, fsdata);
	} else {
		unsigned offset = pos & (PAGE_CACHE_SIZE - 1);
		struct inode *inode = mapping->host;

		flush_dcache_page(page);
		ret = aops->commit_write(file, page, offset, offset+len);
		unlock_page(page);
		mark_page_accessed(page);
		page_cache_release(page);

		if (ret < 0) {
			if (pos + len > inode->i_size)
				vmtruncate(inode, inode->i_size);
		} else if (ret > 0)
			ret = min_t(size_t, copied, ret);
		else
			ret = copied;
	}

	return ret;
}