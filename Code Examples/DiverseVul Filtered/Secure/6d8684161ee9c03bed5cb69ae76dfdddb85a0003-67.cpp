int link(const char *oldpath, const char *newpath)
{
	typedef BOOL (WINAPI *T)(LPCWSTR, LPCWSTR, LPSECURITY_ATTRIBUTES);
	static T create_hard_link = NULL;
	wchar_t woldpath[MAX_PATH], wnewpath[MAX_PATH];
	if (xutftowcs_path(woldpath, oldpath) < 0 ||
		xutftowcs_path(wnewpath, newpath) < 0)
		return -1;

	if (!create_hard_link) {
		create_hard_link = (T) GetProcAddress(
			GetModuleHandle("kernel32.dll"), "CreateHardLinkW");
		if (!create_hard_link)
			create_hard_link = (T)-1;
	}
	if (create_hard_link == (T)-1) {
		errno = ENOSYS;
		return -1;
	}
	if (!create_hard_link(wnewpath, woldpath, NULL)) {
		errno = err_win_to_posix(GetLastError());
		return -1;
	}
	return 0;
}