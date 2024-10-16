static struct vfsmount *get_vfsmount_from_fd(int fd)
{
	struct vfsmount *mnt;

	if (fd == AT_FDCWD) {
		struct fs_struct *fs = current->fs;
		spin_lock(&fs->lock);
		mnt = mntget(fs->pwd.mnt);
		spin_unlock(&fs->lock);
	} else {
		struct fd f = fdget(fd);
		if (!f.file)
			return ERR_PTR(-EBADF);
		mnt = mntget(f.file->f_path.mnt);
		fdput(f);
	}
	return mnt;
}