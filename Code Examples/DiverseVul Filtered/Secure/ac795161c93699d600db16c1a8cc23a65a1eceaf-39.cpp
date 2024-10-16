void nfs_force_use_readdirplus(struct inode *dir)
{
	struct nfs_inode *nfsi = NFS_I(dir);

	if (nfs_server_capable(dir, NFS_CAP_READDIRPLUS) &&
	    !list_empty(&nfsi->open_files)) {
		set_bit(NFS_INO_ADVISE_RDPLUS, &nfsi->flags);
		set_bit(NFS_INO_FORCE_READDIR, &nfsi->flags);
	}
}