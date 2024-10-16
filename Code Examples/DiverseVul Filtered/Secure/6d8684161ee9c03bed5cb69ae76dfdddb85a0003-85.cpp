pid_t waitpid(pid_t pid, int *status, int options)
{
	HANDLE h = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_INFORMATION,
	    FALSE, pid);
	if (!h) {
		errno = ECHILD;
		return -1;
	}

	if (pid > 0 && options & WNOHANG) {
		if (WAIT_OBJECT_0 != WaitForSingleObject(h, 0)) {
			CloseHandle(h);
			return 0;
		}
		options &= ~WNOHANG;
	}

	if (options == 0) {
		struct pinfo_t **ppinfo;
		if (WaitForSingleObject(h, INFINITE) != WAIT_OBJECT_0) {
			CloseHandle(h);
			return 0;
		}

		if (status)
			GetExitCodeProcess(h, (LPDWORD)status);

		EnterCriticalSection(&pinfo_cs);

		ppinfo = &pinfo;
		while (*ppinfo) {
			struct pinfo_t *info = *ppinfo;
			if (info->pid == pid) {
				CloseHandle(info->proc);
				*ppinfo = info->next;
				free(info);
				break;
			}
			ppinfo = &info->next;
		}

		LeaveCriticalSection(&pinfo_cs);

		CloseHandle(h);
		return pid;
	}
	CloseHandle(h);

	errno = EINVAL;
	return -1;
}