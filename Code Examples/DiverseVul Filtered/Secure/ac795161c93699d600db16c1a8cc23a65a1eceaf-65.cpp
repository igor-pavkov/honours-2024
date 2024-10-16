static loff_t nfs_llseek_dir(struct file *filp, loff_t offset, int whence)
{
	struct nfs_open_dir_context *dir_ctx = filp->private_data;

	dfprintk(FILE, "NFS: llseek dir(%pD2, %lld, %d)\n",
			filp, offset, whence);

	switch (whence) {
	default:
		return -EINVAL;
	case SEEK_SET:
		if (offset < 0)
			return -EINVAL;
		spin_lock(&filp->f_lock);
		break;
	case SEEK_CUR:
		if (offset == 0)
			return filp->f_pos;
		spin_lock(&filp->f_lock);
		offset += filp->f_pos;
		if (offset < 0) {
			spin_unlock(&filp->f_lock);
			return -EINVAL;
		}
	}
	if (offset != filp->f_pos) {
		filp->f_pos = offset;
		if (nfs_readdir_use_cookie(filp))
			dir_ctx->dir_cookie = offset;
		else
			dir_ctx->dir_cookie = 0;
		if (offset == 0)
			memset(dir_ctx->verf, 0, sizeof(dir_ctx->verf));
		dir_ctx->duped = 0;
	}
	spin_unlock(&filp->f_lock);
	return offset;
}