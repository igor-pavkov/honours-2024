int nfs_may_open(struct inode *inode, const struct cred *cred, int openflags)
{
	return nfs_do_access(inode, cred, nfs_open_permission_mask(openflags));
}