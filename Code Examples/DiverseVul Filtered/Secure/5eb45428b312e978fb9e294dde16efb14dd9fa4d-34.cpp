static int lxc_get_unused_loop_dev_legacy(char *loop_name)
{
	struct dirent *dp;
	struct loop_info64 lo64;
	DIR *dir;
	int dfd = -1, fd = -1, ret = -1;

	dir = opendir("/dev");
	if (!dir)
		return -1;

	while ((dp = readdir(dir))) {
		if (!dp)
			break;

		if (strncmp(dp->d_name, "loop", 4) != 0)
			continue;

		dfd = dirfd(dir);
		if (dfd < 0)
			continue;

		fd = openat(dfd, dp->d_name, O_RDWR);
		if (fd < 0)
			continue;

		ret = ioctl(fd, LOOP_GET_STATUS64, &lo64);
		if (ret < 0) {
			if (ioctl(fd, LOOP_GET_STATUS64, &lo64) == 0 ||
			    errno != ENXIO) {
				close(fd);
				fd = -1;
				continue;
			}
		}

		ret = snprintf(loop_name, LO_NAME_SIZE, "/dev/%s", dp->d_name);
		if (ret < 0 || ret >= LO_NAME_SIZE) {
			close(fd);
			fd = -1;
			continue;
		}

		break;
	}

	closedir(dir);

	if (fd < 0)
		return -1;

	return fd;
}