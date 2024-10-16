static bool nfs_readdir_array_is_full(struct nfs_cache_array *array)
{
	return array->page_full;
}