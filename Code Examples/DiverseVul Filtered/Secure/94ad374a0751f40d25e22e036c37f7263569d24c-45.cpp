int filemap_fault(struct vm_area_struct *vma, struct vm_fault *vmf)
{
	int error;
	struct file *file = vma->vm_file;
	struct address_space *mapping = file->f_mapping;
	struct file_ra_state *ra = &file->f_ra;
	struct inode *inode = mapping->host;
	struct page *page;
	pgoff_t size;
	int did_readaround = 0;
	int ret = 0;

	size = (i_size_read(inode) + PAGE_CACHE_SIZE - 1) >> PAGE_CACHE_SHIFT;
	if (vmf->pgoff >= size)
		return VM_FAULT_SIGBUS;

	/* If we don't want any read-ahead, don't bother */
	if (VM_RandomReadHint(vma))
		goto no_cached_page;

	/*
	 * Do we have something in the page cache already?
	 */
retry_find:
	page = find_lock_page(mapping, vmf->pgoff);
	/*
	 * For sequential accesses, we use the generic readahead logic.
	 */
	if (VM_SequentialReadHint(vma)) {
		if (!page) {
			page_cache_sync_readahead(mapping, ra, file,
							   vmf->pgoff, 1);
			page = find_lock_page(mapping, vmf->pgoff);
			if (!page)
				goto no_cached_page;
		}
		if (PageReadahead(page)) {
			page_cache_async_readahead(mapping, ra, file, page,
							   vmf->pgoff, 1);
		}
	}

	if (!page) {
		unsigned long ra_pages;

		ra->mmap_miss++;

		/*
		 * Do we miss much more than hit in this file? If so,
		 * stop bothering with read-ahead. It will only hurt.
		 */
		if (ra->mmap_miss > MMAP_LOTSAMISS)
			goto no_cached_page;

		/*
		 * To keep the pgmajfault counter straight, we need to
		 * check did_readaround, as this is an inner loop.
		 */
		if (!did_readaround) {
			ret = VM_FAULT_MAJOR;
			count_vm_event(PGMAJFAULT);
		}
		did_readaround = 1;
		ra_pages = max_sane_readahead(file->f_ra.ra_pages);
		if (ra_pages) {
			pgoff_t start = 0;

			if (vmf->pgoff > ra_pages / 2)
				start = vmf->pgoff - ra_pages / 2;
			do_page_cache_readahead(mapping, file, start, ra_pages);
		}
		page = find_lock_page(mapping, vmf->pgoff);
		if (!page)
			goto no_cached_page;
	}

	if (!did_readaround)
		ra->mmap_miss--;

	/*
	 * We have a locked page in the page cache, now we need to check
	 * that it's up-to-date. If not, it is going to be due to an error.
	 */
	if (unlikely(!PageUptodate(page)))
		goto page_not_uptodate;

	/* Must recheck i_size under page lock */
	size = (i_size_read(inode) + PAGE_CACHE_SIZE - 1) >> PAGE_CACHE_SHIFT;
	if (unlikely(vmf->pgoff >= size)) {
		unlock_page(page);
		page_cache_release(page);
		return VM_FAULT_SIGBUS;
	}

	/*
	 * Found the page and have a reference on it.
	 */
	mark_page_accessed(page);
	ra->prev_pos = (loff_t)page->index << PAGE_CACHE_SHIFT;
	vmf->page = page;
	return ret | VM_FAULT_LOCKED;

no_cached_page:
	/*
	 * We're only likely to ever get here if MADV_RANDOM is in
	 * effect.
	 */
	error = page_cache_read(file, vmf->pgoff);

	/*
	 * The page we want has now been added to the page cache.
	 * In the unlikely event that someone removed it in the
	 * meantime, we'll just come back here and read it again.
	 */
	if (error >= 0)
		goto retry_find;

	/*
	 * An error return from page_cache_read can result if the
	 * system is low on memory, or a problem occurs while trying
	 * to schedule I/O.
	 */
	if (error == -ENOMEM)
		return VM_FAULT_OOM;
	return VM_FAULT_SIGBUS;

page_not_uptodate:
	/* IO error path */
	if (!did_readaround) {
		ret = VM_FAULT_MAJOR;
		count_vm_event(PGMAJFAULT);
	}

	/*
	 * Umm, take care of errors if the page isn't up-to-date.
	 * Try to re-read it _once_. We do this synchronously,
	 * because there really aren't any performance issues here
	 * and we need to check for errors.
	 */
	ClearPageError(page);
	error = mapping->a_ops->readpage(file, page);
	if (!error) {
		wait_on_page_locked(page);
		if (!PageUptodate(page))
			error = -EIO;
	}
	page_cache_release(page);

	if (!error || error == AOP_TRUNCATED_PAGE)
		goto retry_find;

	/* Things didn't work out. Return zero to tell the mm layer so. */
	shrink_readahead_size_eio(file, ra);
	return VM_FAULT_SIGBUS;
}