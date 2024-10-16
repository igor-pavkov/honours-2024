static int try_open(const char *dev, char **devp, int silent)
{
	int fd = open(dev, O_RDWR);
	if (fd != -1) {
		*devp = strdup(dev);
		if (*devp == NULL) {
			fprintf(stderr, "%s: failed to allocate memory\n",
				progname);
			close(fd);
			fd = -1;
		}
	} else if (errno == ENODEV ||
		   errno == ENOENT)/* check for ENOENT too, for the udev case */
		return -2;
	else if (!silent) {
		fprintf(stderr, "%s: failed to open %s: %s\n", progname, dev,
			strerror(errno));
	}
	return fd;
}