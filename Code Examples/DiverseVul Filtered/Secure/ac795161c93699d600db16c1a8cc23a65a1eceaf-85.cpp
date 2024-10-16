static int nfs_readdir_xdr_filler(struct nfs_readdir_descriptor *desc,
				  __be32 *verf, u64 cookie,
				  struct page **pages, size_t bufsize,
				  __be32 *verf_res)
{
	struct inode *inode = file_inode(desc->file);
	struct nfs_readdir_arg arg = {
		.dentry = file_dentry(desc->file),
		.cred = desc->file->f_cred,
		.verf = verf,
		.cookie = cookie,
		.pages = pages,
		.page_len = bufsize,
		.plus = desc->plus,
	};
	struct nfs_readdir_res res = {
		.verf = verf_res,
	};
	unsigned long	timestamp, gencount;
	int		error;

 again:
	timestamp = jiffies;
	gencount = nfs_inc_attr_generation_counter();
	desc->dir_verifier = nfs_save_change_attribute(inode);
	error = NFS_PROTO(inode)->readdir(&arg, &res);
	if (error < 0) {
		/* We requested READDIRPLUS, but the server doesn't grok it */
		if (error == -ENOTSUPP && desc->plus) {
			NFS_SERVER(inode)->caps &= ~NFS_CAP_READDIRPLUS;
			clear_bit(NFS_INO_ADVISE_RDPLUS, &NFS_I(inode)->flags);
			desc->plus = arg.plus = false;
			goto again;
		}
		goto error;
	}
	desc->timestamp = timestamp;
	desc->gencount = gencount;
error:
	return error;
}