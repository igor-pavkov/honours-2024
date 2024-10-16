static void btrfs_cmp_data_free(struct cmp_pages *cmp)
{
	int i;
	struct page *pg;

	for (i = 0; i < cmp->num_pages; i++) {
		pg = cmp->src_pages[i];
		if (pg)
			page_cache_release(pg);
		pg = cmp->dst_pages[i];
		if (pg)
			page_cache_release(pg);
	}
	kfree(cmp->src_pages);
	kfree(cmp->dst_pages);
}