static void btrfs_invalidatepage(struct page *page, unsigned int offset,
				 unsigned int length)
{
	struct inode *inode = page->mapping->host;
	struct extent_io_tree *tree;
	struct btrfs_ordered_extent *ordered;
	struct extent_state *cached_state = NULL;
	u64 page_start = page_offset(page);
	u64 page_end = page_start + PAGE_CACHE_SIZE - 1;
	int inode_evicting = inode->i_state & I_FREEING;

	/*
	 * we have the page locked, so new writeback can't start,
	 * and the dirty bit won't be cleared while we are here.
	 *
	 * Wait for IO on this page so that we can safely clear
	 * the PagePrivate2 bit and do ordered accounting
	 */
	wait_on_page_writeback(page);

	tree = &BTRFS_I(inode)->io_tree;
	if (offset) {
		btrfs_releasepage(page, GFP_NOFS);
		return;
	}

	if (!inode_evicting)
		lock_extent_bits(tree, page_start, page_end, 0, &cached_state);
	ordered = btrfs_lookup_ordered_extent(inode, page_start);
	if (ordered) {
		/*
		 * IO on this page will never be started, so we need
		 * to account for any ordered extents now
		 */
		if (!inode_evicting)
			clear_extent_bit(tree, page_start, page_end,
					 EXTENT_DIRTY | EXTENT_DELALLOC |
					 EXTENT_LOCKED | EXTENT_DO_ACCOUNTING |
					 EXTENT_DEFRAG, 1, 0, &cached_state,
					 GFP_NOFS);
		/*
		 * whoever cleared the private bit is responsible
		 * for the finish_ordered_io
		 */
		if (TestClearPagePrivate2(page)) {
			struct btrfs_ordered_inode_tree *tree;
			u64 new_len;

			tree = &BTRFS_I(inode)->ordered_tree;

			spin_lock_irq(&tree->lock);
			set_bit(BTRFS_ORDERED_TRUNCATED, &ordered->flags);
			new_len = page_start - ordered->file_offset;
			if (new_len < ordered->truncated_len)
				ordered->truncated_len = new_len;
			spin_unlock_irq(&tree->lock);

			if (btrfs_dec_test_ordered_pending(inode, &ordered,
							   page_start,
							   PAGE_CACHE_SIZE, 1))
				btrfs_finish_ordered_io(ordered);
		}
		btrfs_put_ordered_extent(ordered);
		if (!inode_evicting) {
			cached_state = NULL;
			lock_extent_bits(tree, page_start, page_end, 0,
					 &cached_state);
		}
	}

	if (!inode_evicting) {
		clear_extent_bit(tree, page_start, page_end,
				 EXTENT_LOCKED | EXTENT_DIRTY |
				 EXTENT_DELALLOC | EXTENT_DO_ACCOUNTING |
				 EXTENT_DEFRAG, 1, 1,
				 &cached_state, GFP_NOFS);

		__btrfs_releasepage(page, GFP_NOFS);
	}

	ClearPageChecked(page);
	if (PagePrivate(page)) {
		ClearPagePrivate(page);
		set_page_private(page, 0);
		page_cache_release(page);
	}
}