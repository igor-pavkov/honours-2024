static bool nfs_readdir_page_needs_filling(struct page *page)
{
	struct nfs_cache_array *array;
	bool ret;

	array = kmap_atomic(page);
	ret = !nfs_readdir_array_is_full(array);
	kunmap_atomic(array);
	return ret;
}