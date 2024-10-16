GF_Err gf_global_resource_unlock(GF_GlobalLock * lock) {
	if (!lock)
		return GF_BAD_PARAM;
#ifndef WIN32
	assert( lock->pidFile);
	close(lock->fd);
	if (unlink(lock->pidFile))
		perror("Failed to unlink lock file");
	gf_free(lock->pidFile);
	lock->pidFile = NULL;
	lock->fd = -1;
#else /* WIN32 */
	{
		/*MSDN: "The mutex object is destroyed when its last handle has been closed."*/
		BOOL ret = ReleaseMutex(lock->hMutex);
		if (!ret) {
			DWORD err = GetLastError();
			GF_LOG(GF_LOG_ERROR, GF_LOG_MUTEX, ("[Mutex] Couldn't release mutex for global lock: %d\n", err));
		}
		ret = CloseHandle(lock->hMutex);
		if (!ret) {
			DWORD err = GetLastError();
			GF_LOG(GF_LOG_ERROR, GF_LOG_MUTEX, ("[Mutex] Couldn't destroy mutex for global lock: %d\n", err));
		}
	}
#endif
	if (lock->resourceName)
		gf_free(lock->resourceName);
	lock->resourceName = NULL;
	gf_free(lock);
	return GF_OK;
}