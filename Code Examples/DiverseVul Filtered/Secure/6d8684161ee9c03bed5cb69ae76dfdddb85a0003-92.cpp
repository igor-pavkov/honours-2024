int mingw_chmod(const char *filename, int mode)
{
	wchar_t wfilename[MAX_PATH];
	if (xutftowcs_path(wfilename, filename) < 0)
		return -1;
	return _wchmod(wfilename, mode);
}