int pagecache_write_begin(struct file *file, struct address_space *mapping,
				loff_t pos, unsigned len, unsigned flags,
				struct page **pagep, void **fsdata)
{
	const struct address_space_operations *aops = mapping->a_ops;

	if (aops->write_begin) {
		return aops->write_begin(file, mapping, pos, len, flags,
							pagep, fsdata);
	} else {
		int ret;
		pgoff_t index = pos >> PAGE_CACHE_SHIFT;
		unsigned offset = pos & (PAGE_CACHE_SIZE - 1);
		struct inode *inode = mapping->host;
		struct page *page;
again:
		page = __grab_cache_page(mapping, index);
		*pagep = page;
		if (!page)
			return -ENOMEM;

		if (flags & AOP_FLAG_UNINTERRUPTIBLE && !PageUptodate(page)) {
			/*
			 * There is no way to resolve a short write situation
			 * for a !Uptodate page (except by double copying in
			 * the caller done by generic_perform_write_2copy).
			 *
			 * Instead, we have to bring it uptodate here.
			 */
			ret = aops->readpage(file, page);
			page_cache_release(page);
			if (ret) {
				if (ret == AOP_TRUNCATED_PAGE)
					goto again;
				return ret;
			}
			goto again;
		}

		ret = aops->prepare_write(file, page, offset, offset+len);
		if (ret) {
			unlock_page(page);
			page_cache_release(page);
			if (pos + len > inode->i_size)
				vmtruncate(inode, inode->i_size);
		}
		return ret;
	}
}