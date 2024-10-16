static void put_nfs_open_dir_context(struct inode *dir, struct nfs_open_dir_context *ctx)
{
	spin_lock(&dir->i_lock);
	list_del(&ctx->list);
	spin_unlock(&dir->i_lock);
	kfree(ctx);
}