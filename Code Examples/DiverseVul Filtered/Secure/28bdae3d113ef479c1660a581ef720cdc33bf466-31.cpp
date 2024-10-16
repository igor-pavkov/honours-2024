static int try_open_fuse_device(char **devp)
{
	int fd;

	drop_privs();
	fd = try_open(FUSE_DEV, devp, 0);
	restore_privs();
	return fd;
}