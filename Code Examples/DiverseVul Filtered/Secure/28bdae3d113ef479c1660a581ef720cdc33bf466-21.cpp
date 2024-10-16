static int mount_fuse(const char *mnt, const char *opts)
{
	int res;
	int fd;
	char *dev;
	struct stat stbuf;
	char *type = NULL;
	char *source = NULL;
	char *mnt_opts = NULL;
	const char *real_mnt = mnt;
	int mountpoint_fd = -1;

	fd = open_fuse_device(&dev);
	if (fd == -1)
		return -1;

	drop_privs();
	read_conf();

	if (getuid() != 0 && mount_max != -1) {
		int mount_count = count_fuse_fs();
		if (mount_count >= mount_max) {
			fprintf(stderr, "%s: too many FUSE filesystems mounted; mount_max=N can be set in %s\n", progname, FUSE_CONF);
			goto fail_close_fd;
		}
	}

	res = check_perm(&real_mnt, &stbuf, &mountpoint_fd);
	restore_privs();
	if (res != -1)
		res = do_mount(real_mnt, &type, stbuf.st_mode & S_IFMT,
			       fd, opts, dev, &source, &mnt_opts);

	if (mountpoint_fd != -1)
		close(mountpoint_fd);

	if (res == -1)
		goto fail_close_fd;

	res = chdir("/");
	if (res == -1) {
		fprintf(stderr, "%s: failed to chdir to '/'\n", progname);
		goto fail_close_fd;
	}

	if (geteuid() == 0) {
		res = add_mount(source, mnt, type, mnt_opts);
		if (res == -1) {
			/* Can't clean up mount in a non-racy way */
			goto fail_close_fd;
		}
	}

out_free:
	free(source);
	free(type);
	free(mnt_opts);
	free(dev);

	return fd;

fail_close_fd:
	close(fd);
	fd = -1;
	goto out_free;
}