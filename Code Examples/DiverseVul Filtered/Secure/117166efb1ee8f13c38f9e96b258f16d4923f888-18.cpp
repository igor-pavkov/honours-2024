int ext4_write_inline_data_end(struct inode *inode, loff_t pos, unsigned len,
			       unsigned copied, struct page *page)
{
	int ret, no_expand;
	void *kaddr;
	struct ext4_iloc iloc;

	if (unlikely(copied < len)) {
		if (!PageUptodate(page)) {
			copied = 0;
			goto out;
		}
	}

	ret = ext4_get_inode_loc(inode, &iloc);
	if (ret) {
		ext4_std_error(inode->i_sb, ret);
		copied = 0;
		goto out;
	}

	ext4_write_lock_xattr(inode, &no_expand);
	BUG_ON(!ext4_has_inline_data(inode));

	kaddr = kmap_atomic(page);
	ext4_write_inline_data(inode, &iloc, kaddr, pos, len);
	kunmap_atomic(kaddr);
	SetPageUptodate(page);
	/* clear page dirty so that writepages wouldn't work for us. */
	ClearPageDirty(page);

	ext4_write_unlock_xattr(inode, &no_expand);
	brelse(iloc.bh);
out:
	return copied;
}