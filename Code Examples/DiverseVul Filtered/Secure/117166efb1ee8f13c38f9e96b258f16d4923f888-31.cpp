int ext4_try_to_write_inline_data(struct address_space *mapping,
				  struct inode *inode,
				  loff_t pos, unsigned len,
				  unsigned flags,
				  struct page **pagep)
{
	int ret;
	handle_t *handle;
	struct page *page;
	struct ext4_iloc iloc;

	if (pos + len > ext4_get_max_inline_size(inode))
		goto convert;

	ret = ext4_get_inode_loc(inode, &iloc);
	if (ret)
		return ret;

	/*
	 * The possible write could happen in the inode,
	 * so try to reserve the space in inode first.
	 */
	handle = ext4_journal_start(inode, EXT4_HT_INODE, 1);
	if (IS_ERR(handle)) {
		ret = PTR_ERR(handle);
		handle = NULL;
		goto out;
	}

	ret = ext4_prepare_inline_data(handle, inode, pos + len);
	if (ret && ret != -ENOSPC)
		goto out;

	/* We don't have space in inline inode, so convert it to extent. */
	if (ret == -ENOSPC) {
		ext4_journal_stop(handle);
		brelse(iloc.bh);
		goto convert;
	}

	flags |= AOP_FLAG_NOFS;

	page = grab_cache_page_write_begin(mapping, 0, flags);
	if (!page) {
		ret = -ENOMEM;
		goto out;
	}

	*pagep = page;
	down_read(&EXT4_I(inode)->xattr_sem);
	if (!ext4_has_inline_data(inode)) {
		ret = 0;
		unlock_page(page);
		put_page(page);
		goto out_up_read;
	}

	if (!PageUptodate(page)) {
		ret = ext4_read_inline_page(inode, page);
		if (ret < 0)
			goto out_up_read;
	}

	ret = 1;
	handle = NULL;
out_up_read:
	up_read(&EXT4_I(inode)->xattr_sem);
out:
	if (handle)
		ext4_journal_stop(handle);
	brelse(iloc.bh);
	return ret;
convert:
	return ext4_convert_inline_data_to_extent(mapping,
						  inode, flags);
}