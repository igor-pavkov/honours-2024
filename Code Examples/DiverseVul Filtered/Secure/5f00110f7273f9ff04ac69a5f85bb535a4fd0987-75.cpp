static int shmem_getpage_gfp(struct inode *inode, pgoff_t index,
	struct page **pagep, enum sgp_type sgp, gfp_t gfp, int *fault_type)
{
	struct address_space *mapping = inode->i_mapping;
	struct shmem_inode_info *info;
	struct shmem_sb_info *sbinfo;
	struct page *page;
	swp_entry_t swap;
	int error;
	int once = 0;
	int alloced = 0;

	if (index > (MAX_LFS_FILESIZE >> PAGE_CACHE_SHIFT))
		return -EFBIG;
repeat:
	swap.val = 0;
	page = find_lock_page(mapping, index);
	if (radix_tree_exceptional_entry(page)) {
		swap = radix_to_swp_entry(page);
		page = NULL;
	}

	if (sgp != SGP_WRITE && sgp != SGP_FALLOC &&
	    ((loff_t)index << PAGE_CACHE_SHIFT) >= i_size_read(inode)) {
		error = -EINVAL;
		goto failed;
	}

	/* fallocated page? */
	if (page && !PageUptodate(page)) {
		if (sgp != SGP_READ)
			goto clear;
		unlock_page(page);
		page_cache_release(page);
		page = NULL;
	}
	if (page || (sgp == SGP_READ && !swap.val)) {
		*pagep = page;
		return 0;
	}

	/*
	 * Fast cache lookup did not find it:
	 * bring it back from swap or allocate.
	 */
	info = SHMEM_I(inode);
	sbinfo = SHMEM_SB(inode->i_sb);

	if (swap.val) {
		/* Look it up and read it in.. */
		page = lookup_swap_cache(swap);
		if (!page) {
			/* here we actually do the io */
			if (fault_type)
				*fault_type |= VM_FAULT_MAJOR;
			page = shmem_swapin(swap, gfp, info, index);
			if (!page) {
				error = -ENOMEM;
				goto failed;
			}
		}

		/* We have to do this with page locked to prevent races */
		lock_page(page);
		if (!PageSwapCache(page) || page_private(page) != swap.val ||
		    !shmem_confirm_swap(mapping, index, swap)) {
			error = -EEXIST;	/* try again */
			goto unlock;
		}
		if (!PageUptodate(page)) {
			error = -EIO;
			goto failed;
		}
		wait_on_page_writeback(page);

		if (shmem_should_replace_page(page, gfp)) {
			error = shmem_replace_page(&page, gfp, info, index);
			if (error)
				goto failed;
		}

		error = mem_cgroup_cache_charge(page, current->mm,
						gfp & GFP_RECLAIM_MASK);
		if (!error) {
			error = shmem_add_to_page_cache(page, mapping, index,
						gfp, swp_to_radix_entry(swap));
			/*
			 * We already confirmed swap under page lock, and make
			 * no memory allocation here, so usually no possibility
			 * of error; but free_swap_and_cache() only trylocks a
			 * page, so it is just possible that the entry has been
			 * truncated or holepunched since swap was confirmed.
			 * shmem_undo_range() will have done some of the
			 * unaccounting, now delete_from_swap_cache() will do
			 * the rest (including mem_cgroup_uncharge_swapcache).
			 * Reset swap.val? No, leave it so "failed" goes back to
			 * "repeat": reading a hole and writing should succeed.
			 */
			if (error)
				delete_from_swap_cache(page);
		}
		if (error)
			goto failed;

		spin_lock(&info->lock);
		info->swapped--;
		shmem_recalc_inode(inode);
		spin_unlock(&info->lock);

		delete_from_swap_cache(page);
		set_page_dirty(page);
		swap_free(swap);

	} else {
		if (shmem_acct_block(info->flags)) {
			error = -ENOSPC;
			goto failed;
		}
		if (sbinfo->max_blocks) {
			if (percpu_counter_compare(&sbinfo->used_blocks,
						sbinfo->max_blocks) >= 0) {
				error = -ENOSPC;
				goto unacct;
			}
			percpu_counter_inc(&sbinfo->used_blocks);
		}

		page = shmem_alloc_page(gfp, info, index);
		if (!page) {
			error = -ENOMEM;
			goto decused;
		}

		SetPageSwapBacked(page);
		__set_page_locked(page);
		error = mem_cgroup_cache_charge(page, current->mm,
						gfp & GFP_RECLAIM_MASK);
		if (error)
			goto decused;
		error = radix_tree_preload(gfp & GFP_RECLAIM_MASK);
		if (!error) {
			error = shmem_add_to_page_cache(page, mapping, index,
							gfp, NULL);
			radix_tree_preload_end();
		}
		if (error) {
			mem_cgroup_uncharge_cache_page(page);
			goto decused;
		}
		lru_cache_add_anon(page);

		spin_lock(&info->lock);
		info->alloced++;
		inode->i_blocks += BLOCKS_PER_PAGE;
		shmem_recalc_inode(inode);
		spin_unlock(&info->lock);
		alloced = true;

		/*
		 * Let SGP_FALLOC use the SGP_WRITE optimization on a new page.
		 */
		if (sgp == SGP_FALLOC)
			sgp = SGP_WRITE;
clear:
		/*
		 * Let SGP_WRITE caller clear ends if write does not fill page;
		 * but SGP_FALLOC on a page fallocated earlier must initialize
		 * it now, lest undo on failure cancel our earlier guarantee.
		 */
		if (sgp != SGP_WRITE) {
			clear_highpage(page);
			flush_dcache_page(page);
			SetPageUptodate(page);
		}
		if (sgp == SGP_DIRTY)
			set_page_dirty(page);
	}

	/* Perhaps the file has been truncated since we checked */
	if (sgp != SGP_WRITE && sgp != SGP_FALLOC &&
	    ((loff_t)index << PAGE_CACHE_SHIFT) >= i_size_read(inode)) {
		error = -EINVAL;
		if (alloced)
			goto trunc;
		else
			goto failed;
	}
	*pagep = page;
	return 0;

	/*
	 * Error recovery.
	 */
trunc:
	info = SHMEM_I(inode);
	ClearPageDirty(page);
	delete_from_page_cache(page);
	spin_lock(&info->lock);
	info->alloced--;
	inode->i_blocks -= BLOCKS_PER_PAGE;
	spin_unlock(&info->lock);
decused:
	sbinfo = SHMEM_SB(inode->i_sb);
	if (sbinfo->max_blocks)
		percpu_counter_add(&sbinfo->used_blocks, -1);
unacct:
	shmem_unacct_blocks(info->flags, 1);
failed:
	if (swap.val && error != -EINVAL &&
	    !shmem_confirm_swap(mapping, index, swap))
		error = -EEXIST;
unlock:
	if (page) {
		unlock_page(page);
		page_cache_release(page);
	}
	if (error == -ENOSPC && !once++) {
		info = SHMEM_I(inode);
		spin_lock(&info->lock);
		shmem_recalc_inode(inode);
		spin_unlock(&info->lock);
		goto repeat;
	}
	if (error == -EEXIST)	/* from above or from radix_tree_insert */
		goto repeat;
	return error;
}