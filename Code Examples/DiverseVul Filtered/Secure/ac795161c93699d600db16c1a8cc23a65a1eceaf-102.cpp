int nfs_mkdir(struct user_namespace *mnt_userns, struct inode *dir,
	      struct dentry *dentry, umode_t mode)
{
	struct iattr attr;
	int error;

	dfprintk(VFS, "NFS: mkdir(%s/%lu), %pd\n",
			dir->i_sb->s_id, dir->i_ino, dentry);

	attr.ia_valid = ATTR_MODE;
	attr.ia_mode = mode | S_IFDIR;

	trace_nfs_mkdir_enter(dir, dentry);
	error = NFS_PROTO(dir)->mkdir(dir, dentry, &attr);
	trace_nfs_mkdir_exit(dir, dentry, error);
	if (error != 0)
		goto out_err;
	return 0;
out_err:
	d_drop(dentry);
	return error;
}