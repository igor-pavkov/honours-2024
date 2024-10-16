void Secure_Plugin_elevated(char *szPlugin)
{
	char dir[MAX_PATH];
	char exe_file_name[MAX_PATH];
	strcpy_s(dir, " -dsmplugininstance ");
	strcat_s(dir, szPlugin);

	GetModuleFileName(0, exe_file_name, MAX_PATH);
	SHELLEXECUTEINFO shExecInfo;
	shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	shExecInfo.fMask = NULL;
	shExecInfo.hwnd = GetForegroundWindow();
	shExecInfo.lpVerb = "runas";
	shExecInfo.lpFile = exe_file_name;
	shExecInfo.lpParameters = dir;
	shExecInfo.lpDirectory = NULL;
	shExecInfo.nShow = SW_HIDE;
	shExecInfo.hInstApp = NULL;
	ShellExecuteEx(&shExecInfo);
}