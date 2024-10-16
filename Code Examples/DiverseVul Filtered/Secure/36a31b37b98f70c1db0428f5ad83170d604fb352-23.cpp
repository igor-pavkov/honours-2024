void Secure_Save_Plugin_Config(char *szPlugin)
{
	HANDLE hProcess = NULL, hPToken = NULL;
	DWORD id = vncService::GetExplorerLogonPid();
	if (id != 0)
	{
		hProcess = OpenProcess(MAXIMUM_ALLOWED, FALSE, id);
		if (!hProcess) goto error3;
		if (!OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY
			| TOKEN_DUPLICATE | TOKEN_ASSIGN_PRIMARY | TOKEN_ADJUST_SESSIONID
			| TOKEN_READ | TOKEN_WRITE, &hPToken))
		{
			CloseHandle(hProcess);
			goto error3;
		}

		char dir[MAX_PATH];
		char exe_file_name[MAX_PATH];
		GetModuleFileName(0, exe_file_name, MAX_PATH);
		strcpy_s(dir, exe_file_name);
		strcat_s(dir, " -dsmpluginhelper ");
		strcat_s(dir, szPlugin);

		{
			STARTUPINFO          StartUPInfo;
			PROCESS_INFORMATION  ProcessInfo;
			HANDLE Token = NULL;
			HANDLE process = NULL;
			ZeroMemory(&StartUPInfo, sizeof(STARTUPINFO));
			ZeroMemory(&ProcessInfo, sizeof(PROCESS_INFORMATION));
			StartUPInfo.wShowWindow = SW_SHOW;
			StartUPInfo.lpDesktop = "Winsta0\\Default";
			StartUPInfo.cb = sizeof(STARTUPINFO);

			CreateProcessAsUser(hPToken, NULL, dir, NULL, NULL, FALSE, DETACHED_PROCESS, NULL, NULL, &StartUPInfo, &ProcessInfo);
			DWORD errorcode = GetLastError();
			if (errorcode == 1314) goto error1;
			if (process) CloseHandle(process);
			if (Token) CloseHandle(Token);
			if (ProcessInfo.hProcess) CloseHandle(ProcessInfo.hProcess);
			if (ProcessInfo.hThread) CloseHandle(ProcessInfo.hThread);	
			return;
		error1:
			Secure_Plugin(szPlugin);
		}
	error3:
		return;
	}
}