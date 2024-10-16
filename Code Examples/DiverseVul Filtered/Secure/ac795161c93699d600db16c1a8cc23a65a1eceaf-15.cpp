static bool nfs_readdir_array_cookie_in_range(struct nfs_cache_array *array,
					      u64 cookie)
{
	if (!array->cookies_are_ordered)
		return true;
	/* Optimisation for monotonically increasing cookies */
	if (cookie >= array->last_cookie)
		return false;
	if (array->size && cookie < array->array[0].cookie)
		return false;
	return true;
}