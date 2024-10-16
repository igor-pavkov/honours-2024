static int lxc_get_unused_loop_dev(char *name_loop)
{
	int loop_nr, ret;
	int fd_ctl = -1, fd_tmp = -1;

	fd_ctl = open("/dev/loop-control", O_RDWR | O_CLOEXEC);
	if (fd_ctl < 0)
		return -ENODEV;

	loop_nr = ioctl(fd_ctl, LOOP_CTL_GET_FREE);
	if (loop_nr < 0)
		goto on_error;

	ret = snprintf(name_loop, LO_NAME_SIZE, "/dev/loop%d", loop_nr);
	if (ret < 0 || ret >= LO_NAME_SIZE)
		goto on_error;

	fd_tmp = open(name_loop, O_RDWR | O_CLOEXEC);
	if (fd_tmp < 0)
		goto on_error;

on_error:
	close(fd_ctl);
	return fd_tmp;
}