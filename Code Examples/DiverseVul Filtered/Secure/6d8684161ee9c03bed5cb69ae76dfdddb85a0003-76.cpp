int mingw_chdir(const char *dirname)
{
	wchar_t wdirname[MAX_PATH];
	if (xutftowcs_path(wdirname, dirname) < 0)
		return -1;
	return _wchdir(wdirname);
}