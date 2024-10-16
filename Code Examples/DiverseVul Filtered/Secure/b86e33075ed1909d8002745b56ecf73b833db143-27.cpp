static int f2fs_write_data_page(struct page *page,
					struct writeback_control *wbc)
{
	struct inode *inode = page->mapping->host;
	struct f2fs_sb_info *sbi = F2FS_I_SB(inode);
	loff_t i_size = i_size_read(inode);
	const pgoff_t end_index = ((unsigned long long) i_size)
							>> PAGE_SHIFT;
	loff_t psize = (page->index + 1) << PAGE_SHIFT;
	unsigned offset = 0;
	bool need_balance_fs = false;
	int err = 0;
	struct f2fs_io_info fio = {
		.sbi = sbi,
		.type = DATA,
		.op = REQ_OP_WRITE,
		.op_flags = wbc_to_write_flags(wbc),
		.page = page,
		.encrypted_page = NULL,
	};

	trace_f2fs_writepage(page, DATA);

	if (page->index < end_index)
		goto write;

	/*
	 * If the offset is out-of-range of file size,
	 * this page does not have to be written to disk.
	 */
	offset = i_size & (PAGE_SIZE - 1);
	if ((page->index >= end_index + 1) || !offset)
		goto out;

	zero_user_segment(page, offset, PAGE_SIZE);
write:
	if (unlikely(is_sbi_flag_set(sbi, SBI_POR_DOING)))
		goto redirty_out;
	if (f2fs_is_drop_cache(inode))
		goto out;
	/* we should not write 0'th page having journal header */
	if (f2fs_is_volatile_file(inode) && (!page->index ||
			(!wbc->for_reclaim &&
			available_free_memory(sbi, BASE_CHECK))))
		goto redirty_out;

	/* we should bypass data pages to proceed the kworkder jobs */
	if (unlikely(f2fs_cp_error(sbi))) {
		mapping_set_error(page->mapping, -EIO);
		goto out;
	}

	/* Dentry blocks are controlled by checkpoint */
	if (S_ISDIR(inode->i_mode)) {
		err = do_write_data_page(&fio);
		goto done;
	}

	if (!wbc->for_reclaim)
		need_balance_fs = true;
	else if (has_not_enough_free_secs(sbi, 0, 0))
		goto redirty_out;

	err = -EAGAIN;
	f2fs_lock_op(sbi);
	if (f2fs_has_inline_data(inode))
		err = f2fs_write_inline_data(inode, page);
	if (err == -EAGAIN)
		err = do_write_data_page(&fio);
	if (F2FS_I(inode)->last_disk_size < psize)
		F2FS_I(inode)->last_disk_size = psize;
	f2fs_unlock_op(sbi);
done:
	if (err && err != -ENOENT)
		goto redirty_out;

out:
	inode_dec_dirty_pages(inode);
	if (err)
		ClearPageUptodate(page);

	if (wbc->for_reclaim) {
		f2fs_submit_merged_bio_cond(sbi, NULL, page, 0, DATA, WRITE);
		remove_dirty_inode(inode);
	}

	unlock_page(page);
	f2fs_balance_fs(sbi, need_balance_fs);

	if (unlikely(f2fs_cp_error(sbi)))
		f2fs_submit_merged_bio(sbi, DATA, WRITE);

	return 0;

redirty_out:
	redirty_page_for_writepage(wbc, page);
	if (!err)
		return AOP_WRITEPAGE_ACTIVATE;
	unlock_page(page);
	return err;
}