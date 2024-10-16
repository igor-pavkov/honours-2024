static inline int path_has_perm(const struct cred *cred,
				const struct path *path,
				u32 av)
{
	struct inode *inode = d_backing_inode(path->dentry);
	struct common_audit_data ad;

	ad.type = LSM_AUDIT_DATA_PATH;
	ad.u.path = *path;
	__inode_security_revalidate(inode, path->dentry, true);
	return inode_has_perm(cred, inode, av, &ad);
}