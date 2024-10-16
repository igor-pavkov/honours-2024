static int nfs_readdir_array_can_expand(struct nfs_cache_array *array)
{
	struct nfs_cache_array_entry *cache_entry;

	if (array->page_full)
		return -ENOSPC;
	cache_entry = &array->array[array->size + 1];
	if ((char *)cache_entry - (char *)array > PAGE_SIZE) {
		array->page_full = 1;
		return -ENOSPC;
	}
	return 0;
}