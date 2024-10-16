static void nfs_readdir_array_set_eof(struct nfs_cache_array *array)
{
	array->page_is_eof = 1;
	array->page_full = 1;
}