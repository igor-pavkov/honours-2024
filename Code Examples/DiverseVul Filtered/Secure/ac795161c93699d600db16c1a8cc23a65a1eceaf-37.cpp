static int readdir_search_pagecache(struct nfs_readdir_descriptor *desc)
{
	int res;

	if (nfs_readdir_dont_search_cache(desc))
		return -EBADCOOKIE;

	do {
		if (desc->page_index == 0) {
			desc->current_index = 0;
			desc->prev_index = 0;
			desc->last_cookie = 0;
		}
		res = find_and_lock_cache_page(desc);
	} while (res == -EAGAIN);
	return res;
}