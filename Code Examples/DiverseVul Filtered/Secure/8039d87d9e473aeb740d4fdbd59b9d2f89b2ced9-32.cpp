static int cluster_pages_for_defrag(struct inode *inode,
				    struct page **pages,
				    unsigned long start_index,
				    unsigned long num_pages)
{
	unsigned long file_end;
	u64 isize = i_size_read(inode);
	u64 page_start;
	u64 page_end;
	u64 page_cnt;
	int ret;
	int i;
	int i_done;
	struct btrfs_ordered_extent *ordered;
	struct extent_state *cached_state = NULL;
	struct extent_io_tree *tree;
	gfp_t mask = btrfs_alloc_write_mask(inode->i_mapping);

	file_end = (isize - 1) >> PAGE_CACHE_SHIFT;
	if (!isize || start_index > file_end)
		return 0;

	page_cnt = min_t(u64, (u64)num_pages, (u64)file_end - start_index + 1);

	ret = btrfs_delalloc_reserve_space(inode,
					   page_cnt << PAGE_CACHE_SHIFT);
	if (ret)
		return ret;
	i_done = 0;
	tree = &BTRFS_I(inode)->io_tree;

	/* step one, lock all the pages */
	for (i = 0; i < page_cnt; i++) {
		struct page *page;
again:
		page = find_or_create_page(inode->i_mapping,
					   start_index + i, mask);
		if (!page)
			break;

		page_start = page_offset(page);
		page_end = page_start + PAGE_CACHE_SIZE - 1;
		while (1) {
			lock_extent_bits(tree, page_start, page_end,
					 0, &cached_state);
			ordered = btrfs_lookup_ordered_extent(inode,
							      page_start);
			unlock_extent_cached(tree, page_start, page_end,
					     &cached_state, GFP_NOFS);
			if (!ordered)
				break;

			unlock_page(page);
			btrfs_start_ordered_extent(inode, ordered, 1);
			btrfs_put_ordered_extent(ordered);
			lock_page(page);
			/*
			 * we unlocked the page above, so we need check if
			 * it was released or not.
			 */
			if (page->mapping != inode->i_mapping) {
				unlock_page(page);
				page_cache_release(page);
				goto again;
			}
		}

		if (!PageUptodate(page)) {
			btrfs_readpage(NULL, page);
			lock_page(page);
			if (!PageUptodate(page)) {
				unlock_page(page);
				page_cache_release(page);
				ret = -EIO;
				break;
			}
		}

		if (page->mapping != inode->i_mapping) {
			unlock_page(page);
			page_cache_release(page);
			goto again;
		}

		pages[i] = page;
		i_done++;
	}
	if (!i_done || ret)
		goto out;

	if (!(inode->i_sb->s_flags & MS_ACTIVE))
		goto out;

	/*
	 * so now we have a nice long stream of locked
	 * and up to date pages, lets wait on them
	 */
	for (i = 0; i < i_done; i++)
		wait_on_page_writeback(pages[i]);

	page_start = page_offset(pages[0]);
	page_end = page_offset(pages[i_done - 1]) + PAGE_CACHE_SIZE;

	lock_extent_bits(&BTRFS_I(inode)->io_tree,
			 page_start, page_end - 1, 0, &cached_state);
	clear_extent_bit(&BTRFS_I(inode)->io_tree, page_start,
			  page_end - 1, EXTENT_DIRTY | EXTENT_DELALLOC |
			  EXTENT_DO_ACCOUNTING | EXTENT_DEFRAG, 0, 0,
			  &cached_state, GFP_NOFS);

	if (i_done != page_cnt) {
		spin_lock(&BTRFS_I(inode)->lock);
		BTRFS_I(inode)->outstanding_extents++;
		spin_unlock(&BTRFS_I(inode)->lock);
		btrfs_delalloc_release_space(inode,
				     (page_cnt - i_done) << PAGE_CACHE_SHIFT);
	}


	set_extent_defrag(&BTRFS_I(inode)->io_tree, page_start, page_end - 1,
			  &cached_state, GFP_NOFS);

	unlock_extent_cached(&BTRFS_I(inode)->io_tree,
			     page_start, page_end - 1, &cached_state,
			     GFP_NOFS);

	for (i = 0; i < i_done; i++) {
		clear_page_dirty_for_io(pages[i]);
		ClearPageChecked(pages[i]);
		set_page_extent_mapped(pages[i]);
		set_page_dirty(pages[i]);
		unlock_page(pages[i]);
		page_cache_release(pages[i]);
	}
	return i_done;
out:
	for (i = 0; i < i_done; i++) {
		unlock_page(pages[i]);
		page_cache_release(pages[i]);
	}
	btrfs_delalloc_release_space(inode, page_cnt << PAGE_CACHE_SHIFT);
	return ret;

}