static void nfs_readdir_page_init_array(struct page *page, u64 last_cookie)
{
	struct nfs_cache_array *array;

	array = kmap_atomic(page);
	nfs_readdir_array_init(array);
	array->last_cookie = last_cookie;
	array->cookies_are_ordered = 1;
	kunmap_atomic(array);
}