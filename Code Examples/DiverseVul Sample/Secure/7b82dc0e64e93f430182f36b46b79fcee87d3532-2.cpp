asmlinkage long sys_fchmodat(int dfd, const char __user *filename,
			     mode_t mode)
{
	struct nameidata nd;
	struct inode * inode;
	int error;
	struct iattr newattrs;

	error = __user_walk_fd(dfd, filename, LOOKUP_FOLLOW, &nd);
	if (error)
		goto out;
	inode = nd.dentry->d_inode;

	error = -EROFS;
	if (IS_RDONLY(inode))
		goto dput_and_out;

	error = -EPERM;
	if (IS_IMMUTABLE(inode) || IS_APPEND(inode))
		goto dput_and_out;

	mutex_lock(&inode->i_mutex);
	if (mode == (mode_t) -1)
		mode = inode->i_mode;
	newattrs.ia_mode = (mode & S_IALLUGO) | (inode->i_mode & ~S_IALLUGO);
	newattrs.ia_valid = ATTR_MODE | ATTR_CTIME;
	error = notify_change(nd.dentry, &newattrs);
	mutex_unlock(&inode->i_mutex);

dput_and_out:
	path_release(&nd);
out:
	return error;
}