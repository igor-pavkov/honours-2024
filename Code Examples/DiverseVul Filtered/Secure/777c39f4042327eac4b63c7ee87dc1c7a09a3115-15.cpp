void zend_shared_alloc_lock(void)
{
#ifndef ZEND_WIN32

#ifdef ZTS
	tsrm_mutex_lock(zts_lock);
#endif

#if 0
	/* this will happen once per process, and will un-globalize mem_write_lock */
	if (mem_write_lock.l_pid == -1) {
		mem_write_lock.l_pid = getpid();
	}
#endif

	while (1) {
		if (fcntl(lock_file, F_SETLKW, &mem_write_lock) == -1) {
			if (errno == EINTR) {
				continue;
			}
			zend_accel_error(ACCEL_LOG_ERROR, "Cannot create lock - %s (%d)", strerror(errno), errno);
		}
		break;
	}
#else
	zend_shared_alloc_lock_win32();
#endif

	ZCG(locked) = 1;

	/* Prepare translation table
	 *
	 * Make it persistent so that it uses malloc() and allocated blocks
	 * won't be taken from space which is freed by efree in memdup.
	 * Otherwise it leads to false matches in memdup check.
	 */
	zend_hash_init(&xlat_table, 128, NULL, NULL, 1);
}