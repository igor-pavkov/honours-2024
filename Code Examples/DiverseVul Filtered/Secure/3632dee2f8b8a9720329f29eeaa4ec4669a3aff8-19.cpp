SYSCALL_DEFINE2(inotify_rm_watch, int, fd, __s32, wd)
{
	struct file *filp;
	struct inotify_device *dev;
	int ret, fput_needed;

	filp = fget_light(fd, &fput_needed);
	if (unlikely(!filp))
		return -EBADF;

	/* verify that this is indeed an inotify instance */
	if (unlikely(filp->f_op != &inotify_fops)) {
		ret = -EINVAL;
		goto out;
	}

	dev = filp->private_data;

	/* we free our watch data when we get IN_IGNORED */
	ret = inotify_rm_wd(dev->ih, wd);

out:
	fput_light(filp, fput_needed);
	return ret;
}