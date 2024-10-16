static int nfs_readdir(struct file *file, struct dir_context *ctx)
{
	struct dentry	*dentry = file_dentry(file);
	struct inode	*inode = d_inode(dentry);
	struct nfs_inode *nfsi = NFS_I(inode);
	struct nfs_open_dir_context *dir_ctx = file->private_data;
	struct nfs_readdir_descriptor *desc;
	pgoff_t page_index;
	int res;

	dfprintk(FILE, "NFS: readdir(%pD2) starting at cookie %llu\n",
			file, (long long)ctx->pos);
	nfs_inc_stats(inode, NFSIOS_VFSGETDENTS);

	/*
	 * ctx->pos points to the dirent entry number.
	 * *desc->dir_cookie has the cookie for the next entry. We have
	 * to either find the entry with the appropriate number or
	 * revalidate the cookie.
	 */
	if (ctx->pos == 0 || nfs_attribute_cache_expired(inode)) {
		res = nfs_revalidate_mapping(inode, file->f_mapping);
		if (res < 0)
			goto out;
	}

	res = -ENOMEM;
	desc = kzalloc(sizeof(*desc), GFP_KERNEL);
	if (!desc)
		goto out;
	desc->file = file;
	desc->ctx = ctx;
	desc->plus = nfs_use_readdirplus(inode, ctx);

	spin_lock(&file->f_lock);
	desc->dir_cookie = dir_ctx->dir_cookie;
	desc->dup_cookie = dir_ctx->dup_cookie;
	desc->duped = dir_ctx->duped;
	page_index = dir_ctx->page_index;
	desc->attr_gencount = dir_ctx->attr_gencount;
	memcpy(desc->verf, dir_ctx->verf, sizeof(desc->verf));
	spin_unlock(&file->f_lock);

	if (test_and_clear_bit(NFS_INO_FORCE_READDIR, &nfsi->flags) &&
	    list_is_singular(&nfsi->open_files))
		invalidate_mapping_pages(inode->i_mapping, page_index + 1, -1);

	do {
		res = readdir_search_pagecache(desc);

		if (res == -EBADCOOKIE) {
			res = 0;
			/* This means either end of directory */
			if (desc->dir_cookie && !desc->eof) {
				/* Or that the server has 'lost' a cookie */
				res = uncached_readdir(desc);
				if (res == 0)
					continue;
				if (res == -EBADCOOKIE || res == -ENOTSYNC)
					res = 0;
			}
			break;
		}
		if (res == -ETOOSMALL && desc->plus) {
			clear_bit(NFS_INO_ADVISE_RDPLUS, &nfsi->flags);
			nfs_zap_caches(inode);
			desc->page_index = 0;
			desc->plus = false;
			desc->eof = false;
			continue;
		}
		if (res < 0)
			break;

		nfs_do_filldir(desc, nfsi->cookieverf);
		nfs_readdir_page_unlock_and_put_cached(desc);
	} while (!desc->eof);

	spin_lock(&file->f_lock);
	dir_ctx->dir_cookie = desc->dir_cookie;
	dir_ctx->dup_cookie = desc->dup_cookie;
	dir_ctx->duped = desc->duped;
	dir_ctx->attr_gencount = desc->attr_gencount;
	dir_ctx->page_index = desc->page_index;
	memcpy(dir_ctx->verf, desc->verf, sizeof(dir_ctx->verf));
	spin_unlock(&file->f_lock);

	kfree(desc);

out:
	dfprintk(FILE, "NFS: readdir(%pD2) returns %d\n", file, res);
	return res;
}