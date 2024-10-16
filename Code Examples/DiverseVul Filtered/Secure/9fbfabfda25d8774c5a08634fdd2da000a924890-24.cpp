void *read_part_sector(struct parsed_partitions *state, sector_t n, Sector *p)
{
	struct address_space *mapping = state->disk->part0->bd_inode->i_mapping;
	struct page *page;

	if (n >= get_capacity(state->disk)) {
		state->access_beyond_eod = true;
		return NULL;
	}

	page = read_mapping_page(mapping,
			(pgoff_t)(n >> (PAGE_SHIFT - 9)), NULL);
	if (IS_ERR(page))
		goto out;
	if (PageError(page))
		goto out_put_page;

	p->v = page;
	return (unsigned char *)page_address(page) +
			((n & ((1 << (PAGE_SHIFT - 9)) - 1)) << SECTOR_SHIFT);
out_put_page:
	put_page(page);
out:
	p->v = NULL;
	return NULL;
}