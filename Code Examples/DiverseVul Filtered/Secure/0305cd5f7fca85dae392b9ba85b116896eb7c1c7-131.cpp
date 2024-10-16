static int btrfs_writepage_start_hook(struct page *page, u64 start, u64 end)
{
	struct inode *inode = page->mapping->host;
	struct btrfs_writepage_fixup *fixup;
	struct btrfs_root *root = BTRFS_I(inode)->root;

	/* this page is properly in the ordered list */
	if (TestClearPagePrivate2(page))
		return 0;

	if (PageChecked(page))
		return -EAGAIN;

	fixup = kzalloc(sizeof(*fixup), GFP_NOFS);
	if (!fixup)
		return -EAGAIN;

	SetPageChecked(page);
	page_cache_get(page);
	btrfs_init_work(&fixup->work, btrfs_fixup_helper,
			btrfs_writepage_fixup_worker, NULL, NULL);
	fixup->page = page;
	btrfs_queue_work(root->fs_info->fixup_workers, &fixup->work);
	return -EBUSY;
}