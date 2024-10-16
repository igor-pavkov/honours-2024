static int ocfs2_write_zero_page(struct inode *inode, u64 abs_from,
				 u64 abs_to, struct buffer_head *di_bh)
{
	struct address_space *mapping = inode->i_mapping;
	struct page *page;
	unsigned long index = abs_from >> PAGE_SHIFT;
	handle_t *handle;
	int ret = 0;
	unsigned zero_from, zero_to, block_start, block_end;
	struct ocfs2_dinode *di = (struct ocfs2_dinode *)di_bh->b_data;

	BUG_ON(abs_from >= abs_to);
	BUG_ON(abs_to > (((u64)index + 1) << PAGE_SHIFT));
	BUG_ON(abs_from & (inode->i_blkbits - 1));

	handle = ocfs2_zero_start_ordered_transaction(inode, di_bh);
	if (IS_ERR(handle)) {
		ret = PTR_ERR(handle);
		goto out;
	}

	page = find_or_create_page(mapping, index, GFP_NOFS);
	if (!page) {
		ret = -ENOMEM;
		mlog_errno(ret);
		goto out_commit_trans;
	}

	/* Get the offsets within the page that we want to zero */
	zero_from = abs_from & (PAGE_SIZE - 1);
	zero_to = abs_to & (PAGE_SIZE - 1);
	if (!zero_to)
		zero_to = PAGE_SIZE;

	trace_ocfs2_write_zero_page(
			(unsigned long long)OCFS2_I(inode)->ip_blkno,
			(unsigned long long)abs_from,
			(unsigned long long)abs_to,
			index, zero_from, zero_to);

	/* We know that zero_from is block aligned */
	for (block_start = zero_from; block_start < zero_to;
	     block_start = block_end) {
		block_end = block_start + i_blocksize(inode);

		/*
		 * block_start is block-aligned.  Bump it by one to force
		 * __block_write_begin and block_commit_write to zero the
		 * whole block.
		 */
		ret = __block_write_begin(page, block_start + 1, 0,
					  ocfs2_get_block);
		if (ret < 0) {
			mlog_errno(ret);
			goto out_unlock;
		}


		/* must not update i_size! */
		ret = block_commit_write(page, block_start + 1,
					 block_start + 1);
		if (ret < 0)
			mlog_errno(ret);
		else
			ret = 0;
	}

	/*
	 * fs-writeback will release the dirty pages without page lock
	 * whose offset are over inode size, the release happens at
	 * block_write_full_page().
	 */
	i_size_write(inode, abs_to);
	inode->i_blocks = ocfs2_inode_sector_count(inode);
	di->i_size = cpu_to_le64((u64)i_size_read(inode));
	inode->i_mtime = inode->i_ctime = current_time(inode);
	di->i_mtime = di->i_ctime = cpu_to_le64(inode->i_mtime.tv_sec);
	di->i_ctime_nsec = cpu_to_le32(inode->i_mtime.tv_nsec);
	di->i_mtime_nsec = di->i_ctime_nsec;
	if (handle) {
		ocfs2_journal_dirty(handle, di_bh);
		ocfs2_update_inode_fsync_trans(handle, inode, 1);
	}

out_unlock:
	unlock_page(page);
	put_page(page);
out_commit_trans:
	if (handle)
		ocfs2_commit_trans(OCFS2_SB(inode->i_sb), handle);
out:
	return ret;
}