void zend_shared_alloc_create_lock(void)
{
	int val;

#ifdef ZTS
    zts_lock = tsrm_mutex_alloc();
#endif

	sprintf(lockfile_name, "%s/%sXXXXXX", TMP_DIR, SEM_FILENAME_PREFIX);
	lock_file = mkstemp(lockfile_name);
	fchmod(lock_file, 0666);

	if (lock_file == -1) {
		zend_accel_error(ACCEL_LOG_FATAL, "Unable to create lock file: %s (%d)", strerror(errno), errno);
	}
	val = fcntl(lock_file, F_GETFD, 0);
	val |= FD_CLOEXEC;
	fcntl(lock_file, F_SETFD, val);

	unlink(lockfile_name);
}