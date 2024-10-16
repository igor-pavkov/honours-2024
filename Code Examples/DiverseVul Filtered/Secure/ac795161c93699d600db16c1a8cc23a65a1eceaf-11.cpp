static int nfs_readdir_search_for_pos(struct nfs_cache_array *array,
				      struct nfs_readdir_descriptor *desc)
{
	loff_t diff = desc->ctx->pos - desc->current_index;
	unsigned int index;

	if (diff < 0)
		goto out_eof;
	if (diff >= array->size) {
		if (array->page_is_eof)
			goto out_eof;
		return -EAGAIN;
	}

	index = (unsigned int)diff;
	desc->dir_cookie = array->array[index].cookie;
	desc->cache_entry_index = index;
	return 0;
out_eof:
	desc->eof = true;
	return -EBADCOOKIE;
}