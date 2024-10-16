static inline int path_has_perm(const struct cred *cred,
				struct path *path,
				u32 av)
{
	struct inode *inode = path->dentry->d_inode;
	struct common_audit_data ad;

	ad.type = LSM_AUDIT_DATA_PATH;
	ad.u.path = *path;
	return inode_has_perm(cred, inode, av, &ad);
}