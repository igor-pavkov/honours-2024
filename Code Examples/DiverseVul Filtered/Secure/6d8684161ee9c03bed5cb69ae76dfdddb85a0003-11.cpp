int pipe(int filedes[2])
{
	HANDLE h[2];

	/* this creates non-inheritable handles */
	if (!CreatePipe(&h[0], &h[1], NULL, 8192)) {
		errno = err_win_to_posix(GetLastError());
		return -1;
	}
	filedes[0] = _open_osfhandle(HCAST(int, h[0]), O_NOINHERIT);
	if (filedes[0] < 0) {
		CloseHandle(h[0]);
		CloseHandle(h[1]);
		return -1;
	}
	filedes[1] = _open_osfhandle(HCAST(int, h[1]), O_NOINHERIT);
	if (filedes[1] < 0) {
		close(filedes[0]);
		CloseHandle(h[1]);
		return -1;
	}
	return 0;
}