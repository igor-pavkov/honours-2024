static inline int file_path_has_perm(const struct cred *cred,
				     struct file *file,
				     u32 av)
{
	struct common_audit_data ad;

	ad.type = LSM_AUDIT_DATA_PATH;
	ad.u.path = file->f_path;
	return inode_has_perm(cred, file_inode(file), av, &ad);
}