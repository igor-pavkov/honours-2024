long do_handle_open(int mountdirfd,
		    struct file_handle __user *ufh, int open_flag)
{
	long retval = 0;
	struct path path;
	struct file *file;
	int fd;

	retval = handle_to_path(mountdirfd, ufh, &path);
	if (retval)
		return retval;

	fd = get_unused_fd_flags(open_flag);
	if (fd < 0) {
		path_put(&path);
		return fd;
	}
	file = file_open_root(path.dentry, path.mnt, "", open_flag);
	if (IS_ERR(file)) {
		put_unused_fd(fd);
		retval =  PTR_ERR(file);
	} else {
		retval = fd;
		fsnotify_open(file);
		fd_install(fd, file);
	}
	path_put(&path);
	return retval;
}