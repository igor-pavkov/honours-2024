GF_GlobalLock * gf_create_PID_file( const char * resourceName )
{
	const char * prefix = "/gpac_lock_";
	const char * dir = gf_get_default_cache_directory();
	char * pidfile;
	int flags;
	int status;
	pidfile = gf_malloc(strlen(dir)+strlen(prefix)+strlen(resourceName)+1);
	strcpy(pidfile, dir);
	strcat(pidfile, prefix);
	/* Use only valid names for file */
	{
		const char *res;
		char * pid = &(pidfile[strlen(pidfile)]);
		for (res = resourceName; *res ; res++) {
			if (*res >= 'A' && *res <= 'z')
				*pid = * res;
			else
				*pid = '_';
			pid++;
		}
		*pid = '\0';
	}
	int fd = open(pidfile, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	if (fd == -1)
		goto exit;
	/* Get the flags */
	flags = fcntl(fd, F_GETFD);
	if (flags == -1) {
		goto exit;
	}
	/* Set FD_CLOEXEC, so exclusive lock will be removed on exit, so even if GPAC crashes,
	* lock will be allowed for next instance */
	flags |= FD_CLOEXEC;
	/* Now, update the flags */
	if (fcntl(fd, F_SETFD, flags) == -1) {
		goto exit;
	}

	/* Now, we try to lock the file */
	{
		struct flock fl;
		fl.l_type = F_WRLCK;
		fl.l_whence = SEEK_SET;
		fl.l_start = fl.l_len = 0;
		status = fcntl(fd, F_SETLK, &fl);
	}

	if (status == -1) {
		goto exit;
	}

	if (ftruncate(fd, 0) == -1) {
		goto exit;
	}
	/* Write the PID */
	{
		int sz = 100;
		char * buf = gf_malloc( sz );
		sz = snprintf(buf, sz, "%ld\n", (long) getpid());
		if (write(fd, buf, sz) != sz) {
			gf_free(buf);
			goto exit;
		}
	}
	sync();
	{
		GF_GlobalLock * lock = gf_malloc( sizeof(GF_GlobalLock));
		lock->resourceName = gf_strdup(resourceName);
		lock->pidFile = pidfile;
		lock->fd = fd;
		return lock;
	}
exit:
	if (fd >= 0)
		close(fd);
	return NULL;
}