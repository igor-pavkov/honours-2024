static void unlock_umount(int mtablock)
{
	if (mtablock >= 0) {
		int res;

		res = lockf(mtablock, F_ULOCK, 0);
		if (res < 0) {
			fprintf(stderr, "%s: error releasing lock: %s\n",
				progname, strerror(errno));
		}
		close(mtablock);
	}
}