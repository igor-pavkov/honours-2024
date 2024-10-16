void nfs_readdir_clear_array(struct page *page)
{
	struct nfs_cache_array *array;
	int i;

	array = kmap_atomic(page);
	for (i = 0; i < array->size; i++)
		kfree(array->array[i].name);
	nfs_readdir_array_init(array);
	kunmap_atomic(array);
}