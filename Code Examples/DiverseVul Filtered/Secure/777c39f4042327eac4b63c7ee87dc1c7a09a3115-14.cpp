void zend_shared_alloc_unlock(void)
{
	/* Destroy translation table */
	zend_hash_destroy(&xlat_table);

	ZCG(locked) = 0;

#ifndef ZEND_WIN32
	if (fcntl(lock_file, F_SETLK, &mem_write_unlock) == -1) {
		zend_accel_error(ACCEL_LOG_ERROR, "Cannot remove lock - %s (%d)", strerror(errno), errno);
	}
#ifdef ZTS
	tsrm_mutex_unlock(zts_lock);
#endif
#else
	zend_shared_alloc_unlock_win32();
#endif
}