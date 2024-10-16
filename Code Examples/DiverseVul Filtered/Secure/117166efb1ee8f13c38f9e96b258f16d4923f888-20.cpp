static int ext4_read_inline_page(struct inode *inode, struct page *page)
{
	void *kaddr;
	int ret = 0;
	size_t len;
	struct ext4_iloc iloc;

	BUG_ON(!PageLocked(page));
	BUG_ON(!ext4_has_inline_data(inode));
	BUG_ON(page->index);

	if (!EXT4_I(inode)->i_inline_off) {
		ext4_warning(inode->i_sb, "inode %lu doesn't have inline data.",
			     inode->i_ino);
		goto out;
	}

	ret = ext4_get_inode_loc(inode, &iloc);
	if (ret)
		goto out;

	len = min_t(size_t, ext4_get_inline_size(inode), i_size_read(inode));
	kaddr = kmap_atomic(page);
	ret = ext4_read_inline_data(inode, kaddr, len, &iloc);
	flush_dcache_page(page);
	kunmap_atomic(kaddr);
	zero_user_segment(page, len, PAGE_SIZE);
	SetPageUptodate(page);
	brelse(iloc.bh);

out:
	return ret;
}