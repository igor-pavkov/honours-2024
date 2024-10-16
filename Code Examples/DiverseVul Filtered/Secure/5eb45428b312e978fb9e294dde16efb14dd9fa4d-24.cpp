int safe_mount(const char *src, const char *dest, const char *fstype,
		unsigned long flags, const void *data, const char *rootfs)
{
	int srcfd = -1, destfd, ret, saved_errno;
	char srcbuf[50], destbuf[50]; // only needs enough for /proc/self/fd/<fd>
	const char *mntsrc = src;

	if (!rootfs)
		rootfs = "";

	/* todo - allow symlinks for relative paths if 'allowsymlinks' option is passed */
	if (flags & MS_BIND && src && src[0] != '/') {
		INFO("this is a relative bind mount");
		srcfd = open_without_symlink(src, NULL);
		if (srcfd < 0)
			return srcfd;
		ret = snprintf(srcbuf, 50, "/proc/self/fd/%d", srcfd);
		if (ret < 0 || ret > 50) {
			close(srcfd);
			ERROR("Out of memory");
			return -EINVAL;
		}
		mntsrc = srcbuf;
	}

	destfd = open_without_symlink(dest, rootfs);
	if (destfd < 0) {
		if (srcfd != -1) {
			saved_errno = errno;
			close(srcfd);
			errno = saved_errno;
		}
		return destfd;
	}

	ret = snprintf(destbuf, 50, "/proc/self/fd/%d", destfd);
	if (ret < 0 || ret > 50) {
		if (srcfd != -1)
			close(srcfd);
		close(destfd);
		ERROR("Out of memory");
		return -EINVAL;
	}

	ret = mount(mntsrc, destbuf, fstype, flags, data);
	saved_errno = errno;
	if (srcfd != -1)
		close(srcfd);
	close(destfd);
	if (ret < 0) {
		errno = saved_errno;
		SYSERROR("Failed to mount %s onto %s", src ? src : "(null)", dest);
		return ret;
	}

	return 0;
}