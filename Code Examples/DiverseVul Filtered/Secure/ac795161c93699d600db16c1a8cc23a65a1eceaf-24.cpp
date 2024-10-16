static void nfs_set_verifier_locked(struct dentry *dentry, unsigned long verf)
{
	struct inode *inode = d_inode(dentry);
	struct inode *dir = d_inode(dentry->d_parent);

	if (!nfs_verify_change_attribute(dir, verf))
		return;
	if (inode && NFS_PROTO(inode)->have_delegation(inode, FMODE_READ))
		nfs_set_verifier_delegated(&verf);
	dentry->d_time = verf;
}