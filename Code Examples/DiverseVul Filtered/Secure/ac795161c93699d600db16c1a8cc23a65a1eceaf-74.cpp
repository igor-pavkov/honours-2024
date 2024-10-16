static int nfs_readdir_search_array(struct nfs_readdir_descriptor *desc)
{
	struct nfs_cache_array *array;
	int status;

	array = kmap_atomic(desc->page);

	if (desc->dir_cookie == 0)
		status = nfs_readdir_search_for_pos(array, desc);
	else
		status = nfs_readdir_search_for_cookie(array, desc);

	if (status == -EAGAIN) {
		desc->last_cookie = array->last_cookie;
		desc->current_index += array->size;
		desc->page_index++;
	}
	kunmap_atomic(array);
	return status;
}