static pid_t mingw_spawnve_fd(const char *cmd, const char **argv, char **deltaenv,
			      const char *dir,
			      int prepend_cmd, int fhin, int fhout, int fherr)
{
	STARTUPINFOW si;
	PROCESS_INFORMATION pi;
	struct strbuf args;
	wchar_t wcmd[MAX_PATH], wdir[MAX_PATH], *wargs, *wenvblk = NULL;
	unsigned flags = CREATE_UNICODE_ENVIRONMENT;
	BOOL ret;

	/* Determine whether or not we are associated to a console */
	HANDLE cons = CreateFile("CONOUT$", GENERIC_WRITE,
			FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL, NULL);
	if (cons == INVALID_HANDLE_VALUE) {
		/* There is no console associated with this process.
		 * Since the child is a console process, Windows
		 * would normally create a console window. But
		 * since we'll be redirecting std streams, we do
		 * not need the console.
		 * It is necessary to use DETACHED_PROCESS
		 * instead of CREATE_NO_WINDOW to make ssh
		 * recognize that it has no console.
		 */
		flags |= DETACHED_PROCESS;
	} else {
		/* There is already a console. If we specified
		 * DETACHED_PROCESS here, too, Windows would
		 * disassociate the child from the console.
		 * The same is true for CREATE_NO_WINDOW.
		 * Go figure!
		 */
		CloseHandle(cons);
	}
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESTDHANDLES;
	si.hStdInput = winansi_get_osfhandle(fhin);
	si.hStdOutput = winansi_get_osfhandle(fhout);
	si.hStdError = winansi_get_osfhandle(fherr);

	if (xutftowcs_path(wcmd, cmd) < 0)
		return -1;
	if (dir && xutftowcs_path(wdir, dir) < 0)
		return -1;

	/* concatenate argv, quoting args as we go */
	strbuf_init(&args, 0);
	if (prepend_cmd) {
		char *quoted = (char *)quote_arg(cmd);
		strbuf_addstr(&args, quoted);
		if (quoted != cmd)
			free(quoted);
	}
	for (; *argv; argv++) {
		char *quoted = (char *)quote_arg(*argv);
		if (*args.buf)
			strbuf_addch(&args, ' ');
		strbuf_addstr(&args, quoted);
		if (quoted != *argv)
			free(quoted);
	}

	ALLOC_ARRAY(wargs, st_add(st_mult(2, args.len), 1));
	xutftowcs(wargs, args.buf, 2 * args.len + 1);
	strbuf_release(&args);

	wenvblk = make_environment_block(deltaenv);

	memset(&pi, 0, sizeof(pi));
	ret = CreateProcessW(wcmd, wargs, NULL, NULL, TRUE, flags,
		wenvblk, dir ? wdir : NULL, &si, &pi);

	free(wenvblk);
	free(wargs);

	if (!ret) {
		errno = ENOENT;
		return -1;
	}
	CloseHandle(pi.hThread);

	/*
	 * The process ID is the human-readable identifier of the process
	 * that we want to present in log and error messages. The handle
	 * is not useful for this purpose. But we cannot close it, either,
	 * because it is not possible to turn a process ID into a process
	 * handle after the process terminated.
	 * Keep the handle in a list for waitpid.
	 */
	EnterCriticalSection(&pinfo_cs);
	{
		struct pinfo_t *info = xmalloc(sizeof(struct pinfo_t));
		info->pid = pi.dwProcessId;
		info->proc = pi.hProcess;
		info->next = pinfo;
		pinfo = info;
	}
	LeaveCriticalSection(&pinfo_cs);

	return (pid_t)pi.dwProcessId;
}