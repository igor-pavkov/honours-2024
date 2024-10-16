static int unmount_fuse(const char *mnt, int quiet, int lazy)
{
	int res;
	int mtablock = lock_umount();

	res = unmount_fuse_locked(mnt, quiet, lazy);
	unlock_umount(mtablock);

	return res;
}