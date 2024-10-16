struct file *dentry_open(struct dentry *dentry, struct vfsmount *mnt, int flags)
{
	int error;
	struct file *f;

	error = -ENFILE;
	f = get_empty_filp();
	if (f == NULL) {
		dput(dentry);
		mntput(mnt);
		return ERR_PTR(error);
	}

	return __dentry_open(dentry, mnt, flags, f, NULL);
}