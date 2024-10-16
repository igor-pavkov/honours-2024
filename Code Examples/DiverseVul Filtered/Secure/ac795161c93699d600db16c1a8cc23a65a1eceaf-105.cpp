static void nfs_readdir_page_set_eof(struct page *page)
{
	struct nfs_cache_array *array;

	array = kmap_atomic(page);
	nfs_readdir_array_set_eof(array);
	kunmap_atomic(array);
}