bool nfs_use_readdirplus(struct inode *dir, struct dir_context *ctx)
{
	if (!nfs_server_capable(dir, NFS_CAP_READDIRPLUS))
		return false;
	if (test_and_clear_bit(NFS_INO_ADVISE_RDPLUS, &NFS_I(dir)->flags))
		return true;
	if (ctx->pos == 0)
		return true;
	return false;
}