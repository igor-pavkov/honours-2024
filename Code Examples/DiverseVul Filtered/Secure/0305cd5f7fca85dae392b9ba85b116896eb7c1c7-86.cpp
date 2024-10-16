static int lock_extent_direct(struct inode *inode, u64 lockstart, u64 lockend,
			      struct extent_state **cached_state, int writing)
{
	struct btrfs_ordered_extent *ordered;
	int ret = 0;

	while (1) {
		lock_extent_bits(&BTRFS_I(inode)->io_tree, lockstart, lockend,
				 0, cached_state);
		/*
		 * We're concerned with the entire range that we're going to be
		 * doing DIO to, so we need to make sure theres no ordered
		 * extents in this range.
		 */
		ordered = btrfs_lookup_ordered_range(inode, lockstart,
						     lockend - lockstart + 1);

		/*
		 * We need to make sure there are no buffered pages in this
		 * range either, we could have raced between the invalidate in
		 * generic_file_direct_write and locking the extent.  The
		 * invalidate needs to happen so that reads after a write do not
		 * get stale data.
		 */
		if (!ordered &&
		    (!writing ||
		     !btrfs_page_exists_in_range(inode, lockstart, lockend)))
			break;

		unlock_extent_cached(&BTRFS_I(inode)->io_tree, lockstart, lockend,
				     cached_state, GFP_NOFS);

		if (ordered) {
			btrfs_start_ordered_extent(inode, ordered, 1);
			btrfs_put_ordered_extent(ordered);
		} else {
			/* Screw you mmap */
			ret = btrfs_fdatawrite_range(inode, lockstart, lockend);
			if (ret)
				break;
			ret = filemap_fdatawait_range(inode->i_mapping,
						      lockstart,
						      lockend);
			if (ret)
				break;

			/*
			 * If we found a page that couldn't be invalidated just
			 * fall back to buffered.
			 */
			ret = invalidate_inode_pages2_range(inode->i_mapping,
					lockstart >> PAGE_CACHE_SHIFT,
					lockend >> PAGE_CACHE_SHIFT);
			if (ret)
				break;
		}

		cond_resched();
	}

	return ret;
}