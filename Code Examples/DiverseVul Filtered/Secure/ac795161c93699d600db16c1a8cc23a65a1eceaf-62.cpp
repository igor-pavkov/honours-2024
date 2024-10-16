static u64 nfs_readdir_page_last_cookie(struct page *page)
{
	struct nfs_cache_array *array;
	u64 ret;

	array = kmap_atomic(page);
	ret = array->last_cookie;
	kunmap_atomic(array);
	return ret;
}