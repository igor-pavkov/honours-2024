static void nfs_do_filldir(struct nfs_readdir_descriptor *desc,
			   const __be32 *verf)
{
	struct file	*file = desc->file;
	struct nfs_cache_array *array;
	unsigned int i = 0;

	array = kmap(desc->page);
	for (i = desc->cache_entry_index; i < array->size; i++) {
		struct nfs_cache_array_entry *ent;

		ent = &array->array[i];
		if (!dir_emit(desc->ctx, ent->name, ent->name_len,
		    nfs_compat_user_ino64(ent->ino), ent->d_type)) {
			desc->eof = true;
			break;
		}
		memcpy(desc->verf, verf, sizeof(desc->verf));
		if (i < (array->size-1))
			desc->dir_cookie = array->array[i+1].cookie;
		else
			desc->dir_cookie = array->last_cookie;
		if (nfs_readdir_use_cookie(file))
			desc->ctx->pos = desc->dir_cookie;
		else
			desc->ctx->pos++;
		if (desc->duped != 0)
			desc->duped = 1;
	}
	if (array->page_is_eof)
		desc->eof = true;

	kunmap(desc->page);
	dfprintk(DIRCACHE, "NFS: nfs_do_filldir() filling ended @ cookie %llu\n",
			(unsigned long long)desc->dir_cookie);
}