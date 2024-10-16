static void nfs_readdir_array_init(struct nfs_cache_array *array)
{
	memset(array, 0, sizeof(struct nfs_cache_array));
}