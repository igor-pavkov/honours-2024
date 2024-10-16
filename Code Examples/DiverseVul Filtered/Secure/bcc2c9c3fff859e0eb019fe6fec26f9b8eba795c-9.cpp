static void kvp_release_lock(int pool)
{
	struct flock fl = {F_UNLCK, SEEK_SET, 0, 0, 0};
	fl.l_pid = getpid();

	if (fcntl(kvp_file_info[pool].fd, F_SETLK, &fl) == -1) {
		perror("fcntl");
		syslog(LOG_ERR, "Failed to release the lock pool: %d", pool);
		exit(-1);
	}
}