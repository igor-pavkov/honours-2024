static int unmount_fuse(const char *mnt, int quiet, int lazy)
{
	(void) quiet;
	return fuse_mnt_umount(progname, mnt, mnt, lazy);
}