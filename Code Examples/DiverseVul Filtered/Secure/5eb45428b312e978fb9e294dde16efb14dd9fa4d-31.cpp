int lxc_prepare_loop_dev(const char *source, char *loop_dev, int flags)
{
	int ret;
	struct loop_info64 lo64;
	int fd_img = -1, fret = -1, fd_loop = -1;

	fd_loop = lxc_get_unused_loop_dev(loop_dev);
	if (fd_loop < 0) {
		if (fd_loop == -ENODEV)
			fd_loop = lxc_get_unused_loop_dev_legacy(loop_dev);
		else
			goto on_error;
	}

	fd_img = open(source, O_RDWR | O_CLOEXEC);
	if (fd_img < 0)
		goto on_error;

	ret = ioctl(fd_loop, LOOP_SET_FD, fd_img);
	if (ret < 0)
		goto on_error;

	memset(&lo64, 0, sizeof(lo64));
	lo64.lo_flags = flags;

	ret = ioctl(fd_loop, LOOP_SET_STATUS64, &lo64);
	if (ret < 0)
		goto on_error;

	fret = 0;

on_error:
	if (fd_img >= 0)
		close(fd_img);

	if (fret < 0 && fd_loop >= 0) {
		close(fd_loop);
		fd_loop = -1;
	}

	return fd_loop;
}