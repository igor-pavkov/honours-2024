static ssize_t generic_perform_write_2copy(struct file *file,
				struct iov_iter *i, loff_t pos)
{
	struct address_space *mapping = file->f_mapping;
	const struct address_space_operations *a_ops = mapping->a_ops;
	struct inode *inode = mapping->host;
	long status = 0;
	ssize_t written = 0;

	do {
		struct page *src_page;
		struct page *page;
		pgoff_t index;		/* Pagecache index for current page */
		unsigned long offset;	/* Offset into pagecache page */
		unsigned long bytes;	/* Bytes to write to page */
		size_t copied;		/* Bytes copied from user */

		offset = (pos & (PAGE_CACHE_SIZE - 1));
		index = pos >> PAGE_CACHE_SHIFT;
		bytes = min_t(unsigned long, PAGE_CACHE_SIZE - offset,
						iov_iter_count(i));

		/*
		 * a non-NULL src_page indicates that we're doing the
		 * copy via get_user_pages and kmap.
		 */
		src_page = NULL;

		/*
		 * Bring in the user page that we will copy from _first_.
		 * Otherwise there's a nasty deadlock on copying from the
		 * same page as we're writing to, without it being marked
		 * up-to-date.
		 *
		 * Not only is this an optimisation, but it is also required
		 * to check that the address is actually valid, when atomic
		 * usercopies are used, below.
		 */
		if (unlikely(iov_iter_fault_in_readable(i, bytes))) {
			status = -EFAULT;
			break;
		}

		page = __grab_cache_page(mapping, index);
		if (!page) {
			status = -ENOMEM;
			break;
		}

		/*
		 * non-uptodate pages cannot cope with short copies, and we
		 * cannot take a pagefault with the destination page locked.
		 * So pin the source page to copy it.
		 */
		if (!PageUptodate(page) && !segment_eq(get_fs(), KERNEL_DS)) {
			unlock_page(page);

			src_page = alloc_page(GFP_KERNEL);
			if (!src_page) {
				page_cache_release(page);
				status = -ENOMEM;
				break;
			}

			/*
			 * Cannot get_user_pages with a page locked for the
			 * same reason as we can't take a page fault with a
			 * page locked (as explained below).
			 */
			copied = iov_iter_copy_from_user(src_page, i,
								offset, bytes);
			if (unlikely(copied == 0)) {
				status = -EFAULT;
				page_cache_release(page);
				page_cache_release(src_page);
				break;
			}
			bytes = copied;

			lock_page(page);
			/*
			 * Can't handle the page going uptodate here, because
			 * that means we would use non-atomic usercopies, which
			 * zero out the tail of the page, which can cause
			 * zeroes to become transiently visible. We could just
			 * use a non-zeroing copy, but the APIs aren't too
			 * consistent.
			 */
			if (unlikely(!page->mapping || PageUptodate(page))) {
				unlock_page(page);
				page_cache_release(page);
				page_cache_release(src_page);
				continue;
			}
		}

		status = a_ops->prepare_write(file, page, offset, offset+bytes);
		if (unlikely(status))
			goto fs_write_aop_error;

		if (!src_page) {
			/*
			 * Must not enter the pagefault handler here, because
			 * we hold the page lock, so we might recursively
			 * deadlock on the same lock, or get an ABBA deadlock
			 * against a different lock, or against the mmap_sem
			 * (which nests outside the page lock).  So increment
			 * preempt count, and use _atomic usercopies.
			 *
			 * The page is uptodate so we are OK to encounter a
			 * short copy: if unmodified parts of the page are
			 * marked dirty and written out to disk, it doesn't
			 * really matter.
			 */
			pagefault_disable();
			copied = iov_iter_copy_from_user_atomic(page, i,
								offset, bytes);
			pagefault_enable();
		} else {
			void *src, *dst;
			src = kmap_atomic(src_page, KM_USER0);
			dst = kmap_atomic(page, KM_USER1);
			memcpy(dst + offset, src + offset, bytes);
			kunmap_atomic(dst, KM_USER1);
			kunmap_atomic(src, KM_USER0);
			copied = bytes;
		}
		flush_dcache_page(page);

		status = a_ops->commit_write(file, page, offset, offset+bytes);
		if (unlikely(status < 0))
			goto fs_write_aop_error;
		if (unlikely(status > 0)) /* filesystem did partial write */
			copied = min_t(size_t, copied, status);

		unlock_page(page);
		mark_page_accessed(page);
		page_cache_release(page);
		if (src_page)
			page_cache_release(src_page);

		iov_iter_advance(i, copied);
		pos += copied;
		written += copied;

		balance_dirty_pages_ratelimited(mapping);
		cond_resched();
		continue;

fs_write_aop_error:
		unlock_page(page);
		page_cache_release(page);
		if (src_page)
			page_cache_release(src_page);

		/*
		 * prepare_write() may have instantiated a few blocks
		 * outside i_size.  Trim these off again. Don't need
		 * i_size_read because we hold i_mutex.
		 */
		if (pos + bytes > inode->i_size)
			vmtruncate(inode, inode->i_size);
		break;
	} while (iov_iter_count(i));

	return written ? written : status;
}