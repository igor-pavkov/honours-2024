static int nfs_readdir_search_for_cookie(struct nfs_cache_array *array,
					 struct nfs_readdir_descriptor *desc)
{
	int i;
	loff_t new_pos;
	int status = -EAGAIN;

	if (!nfs_readdir_array_cookie_in_range(array, desc->dir_cookie))
		goto check_eof;

	for (i = 0; i < array->size; i++) {
		if (array->array[i].cookie == desc->dir_cookie) {
			struct nfs_inode *nfsi = NFS_I(file_inode(desc->file));

			new_pos = desc->current_index + i;
			if (desc->attr_gencount != nfsi->attr_gencount ||
			    !nfs_readdir_inode_mapping_valid(nfsi)) {
				desc->duped = 0;
				desc->attr_gencount = nfsi->attr_gencount;
			} else if (new_pos < desc->prev_index) {
				if (desc->duped > 0
				    && desc->dup_cookie == desc->dir_cookie) {
					if (printk_ratelimit()) {
						pr_notice("NFS: directory %pD2 contains a readdir loop."
								"Please contact your server vendor.  "
								"The file: %s has duplicate cookie %llu\n",
								desc->file, array->array[i].name, desc->dir_cookie);
					}
					status = -ELOOP;
					goto out;
				}
				desc->dup_cookie = desc->dir_cookie;
				desc->duped = -1;
			}
			if (nfs_readdir_use_cookie(desc->file))
				desc->ctx->pos = desc->dir_cookie;
			else
				desc->ctx->pos = new_pos;
			desc->prev_index = new_pos;
			desc->cache_entry_index = i;
			return 0;
		}
	}
check_eof:
	if (array->page_is_eof) {
		status = -EBADCOOKIE;
		if (desc->dir_cookie == array->last_cookie)
			desc->eof = true;
	}
out:
	return status;
}