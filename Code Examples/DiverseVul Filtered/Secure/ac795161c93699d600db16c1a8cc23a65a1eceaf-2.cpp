static int nfs_dentry_verify_change(struct inode *dir, struct dentry *dentry)
{
	if (nfs_server_capable(dir, NFS_CAP_CASE_INSENSITIVE) &&
	    d_really_is_negative(dentry))
		return dentry->d_time == inode_peek_iversion_raw(dir);
	return nfs_verify_change_attribute(dir, dentry->d_time);
}