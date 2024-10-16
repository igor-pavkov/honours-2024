SYSCALL_DEFINE3(inotify_add_watch, int, fd, const char __user *, pathname,
		u32, mask)
{
	struct inode *inode;
	struct inotify_device *dev;
	struct path path;
	struct file *filp;
	int ret, fput_needed;
	unsigned flags = 0;

	filp = fget_light(fd, &fput_needed);
	if (unlikely(!filp))
		return -EBADF;

	/* verify that this is indeed an inotify instance */
	if (unlikely(filp->f_op != &inotify_fops)) {
		ret = -EINVAL;
		goto fput_and_out;
	}

	if (!(mask & IN_DONT_FOLLOW))
		flags |= LOOKUP_FOLLOW;
	if (mask & IN_ONLYDIR)
		flags |= LOOKUP_DIRECTORY;

	ret = find_inode(pathname, &path, flags);
	if (unlikely(ret))
		goto fput_and_out;

	/* inode held in place by reference to path; dev by fget on fd */
	inode = path.dentry->d_inode;
	dev = filp->private_data;

	mutex_lock(&dev->up_mutex);
	ret = inotify_find_update_watch(dev->ih, inode, mask);
	if (ret == -ENOENT)
		ret = create_watch(dev, inode, mask);
	mutex_unlock(&dev->up_mutex);

	path_put(&path);
fput_and_out:
	fput_light(filp, fput_needed);
	return ret;
}