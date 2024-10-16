static inline int dentry_has_perm(const struct cred *cred,
				  struct dentry *dentry,
				  u32 av)
{
	struct inode *inode = d_backing_inode(dentry);
	struct common_audit_data ad;

	ad.type = LSM_AUDIT_DATA_DENTRY;
	ad.u.dentry = dentry;
	__inode_security_revalidate(inode, dentry, true);
	return inode_has_perm(cred, inode, av, &ad);
}