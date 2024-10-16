static int btrfs_cmp_data_prepare(struct inode *src, u64 loff,
				  struct inode *dst, u64 dst_loff,
				  u64 len, struct cmp_pages *cmp)
{
	int ret;
	int num_pages = PAGE_CACHE_ALIGN(len) >> PAGE_CACHE_SHIFT;
	struct page **src_pgarr, **dst_pgarr;

	/*
	 * We must gather up all the pages before we initiate our
	 * extent locking. We use an array for the page pointers. Size
	 * of the array is bounded by len, which is in turn bounded by
	 * BTRFS_MAX_DEDUPE_LEN.
	 */
	src_pgarr = kzalloc(num_pages * sizeof(struct page *), GFP_NOFS);
	dst_pgarr = kzalloc(num_pages * sizeof(struct page *), GFP_NOFS);
	if (!src_pgarr || !dst_pgarr) {
		kfree(src_pgarr);
		kfree(dst_pgarr);
		return -ENOMEM;
	}
	cmp->num_pages = num_pages;
	cmp->src_pages = src_pgarr;
	cmp->dst_pages = dst_pgarr;

	ret = gather_extent_pages(src, cmp->src_pages, cmp->num_pages, loff);
	if (ret)
		goto out;

	ret = gather_extent_pages(dst, cmp->dst_pages, cmp->num_pages, dst_loff);

out:
	if (ret)
		btrfs_cmp_data_free(cmp);
	return 0;
}