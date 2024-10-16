static struct nfs_open_dir_context *alloc_nfs_open_dir_context(struct inode *dir)
{
	struct nfs_inode *nfsi = NFS_I(dir);
	struct nfs_open_dir_context *ctx;
	ctx = kmalloc(sizeof(*ctx), GFP_KERNEL);
	if (ctx != NULL) {
		ctx->duped = 0;
		ctx->attr_gencount = nfsi->attr_gencount;
		ctx->dir_cookie = 0;
		ctx->dup_cookie = 0;
		ctx->page_index = 0;
		spin_lock(&dir->i_lock);
		if (list_empty(&nfsi->open_files) &&
		    (nfsi->cache_validity & NFS_INO_DATA_INVAL_DEFER))
			nfs_set_cache_invalid(dir,
					      NFS_INO_INVALID_DATA |
						      NFS_INO_REVAL_FORCED);
		list_add(&ctx->list, &nfsi->open_files);
		clear_bit(NFS_INO_FORCE_READDIR, &nfsi->flags);
		spin_unlock(&dir->i_lock);
		return ctx;
	}
	return  ERR_PTR(-ENOMEM);
}