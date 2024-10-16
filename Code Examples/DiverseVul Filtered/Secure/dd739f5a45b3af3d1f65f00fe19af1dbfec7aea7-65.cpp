static void r_w32_cmd_pipe(RCore *core, char *radare_cmd, char *shell_cmd) {
	STARTUPINFO si = {0};
	PROCESS_INFORMATION pi = {0};
	SECURITY_ATTRIBUTES sa;
	HANDLE pipe[2] = {NULL, NULL};
	int fd_out = -1, cons_out = -1;
	char *_shell_cmd = NULL;
	LPTSTR _shell_cmd_ = NULL;
	DWORD mode;
	GetConsoleMode (GetStdHandle (STD_OUTPUT_HANDLE), &mode);

	sa.nLength = sizeof (SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;
	if (!CreatePipe (&pipe[0], &pipe[1], &sa, 0)) {
		r_sys_perror ("r_w32_cmd_pipe/CreatePipe");
		goto err_r_w32_cmd_pipe;
	}
	if (!SetHandleInformation (pipe[1], HANDLE_FLAG_INHERIT, 0)) {
		r_sys_perror ("r_w32_cmd_pipe/SetHandleInformation");
		goto err_r_w32_cmd_pipe;
	}
	si.hStdError = GetStdHandle (STD_ERROR_HANDLE);
	si.hStdOutput = GetStdHandle (STD_OUTPUT_HANDLE);
	si.hStdInput = pipe[0];
	si.dwFlags |= STARTF_USESTDHANDLES;
	si.cb = sizeof (si);
	_shell_cmd = shell_cmd;
	while (*_shell_cmd && isspace ((ut8)*_shell_cmd)) {
		_shell_cmd++;
	}
	char *tmp = r_str_newf ("/Q /c \"%s\"", shell_cmd);
	if (!tmp) {
		goto err_r_w32_cmd_pipe;
	}
	_shell_cmd = tmp;
	_shell_cmd_ = r_sys_conv_utf8_to_win (_shell_cmd);
	free (tmp);
	if (!_shell_cmd_) {
		goto err_r_w32_cmd_pipe;
	}
	TCHAR *systemdir = calloc (MAX_PATH, sizeof (TCHAR));
	if (!systemdir) {
		goto err_r_w32_cmd_pipe;
	}
	int ret = GetSystemDirectory (systemdir, MAX_PATH);
	if (!ret) {
		r_sys_perror ("r_w32_cmd_pipe/systemdir");
		goto err_r_w32_cmd_pipe;
	}
	_tcscat_s (systemdir, MAX_PATH, TEXT("\\cmd.exe"));
	// exec windows process
	if (!CreateProcess (systemdir, _shell_cmd_, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
		r_sys_perror ("r_w32_cmd_pipe/CreateProcess");
		goto err_r_w32_cmd_pipe;
	}
	fd_out = _open_osfhandle ((intptr_t)pipe[1], _O_WRONLY|_O_TEXT);
	if (fd_out == -1) {
		perror ("_open_osfhandle");
		goto err_r_w32_cmd_pipe;
	}
	cons_out = dup (1);
	dup2 (fd_out, 1);
	// exec radare command
	r_core_cmd (core, radare_cmd, 0);
	r_cons_flush ();
	close (1);
	close (fd_out);
	fd_out = -1;
	WaitForSingleObject (pi.hProcess, INFINITE);
err_r_w32_cmd_pipe:
	if (pi.hProcess) {
		CloseHandle (pi.hProcess);
	}
	if (pi.hThread) {
		CloseHandle (pi.hThread);
	}
	if (pipe[0]) {
		CloseHandle (pipe[0]);
	}
	if (pipe[1]) {
		CloseHandle (pipe[1]);
	}
	if (fd_out != -1) {
		close (fd_out);
	}
	if (cons_out != -1) {
		dup2 (cons_out, 1);
		close (cons_out);
	}
	free (systemdir);
	free (_shell_cmd_);
	SetConsoleMode (GetStdHandle (STD_OUTPUT_HANDLE), mode);
}