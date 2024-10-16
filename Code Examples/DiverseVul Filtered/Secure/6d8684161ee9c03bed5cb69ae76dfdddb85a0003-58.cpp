int mingw_access(const char *filename, int mode)
{
	wchar_t wfilename[MAX_PATH];
	if (xutftowcs_path(wfilename, filename) < 0)
		return -1;
	/* X_OK is not supported by the MSVCRT version */
	return _waccess(wfilename, mode & ~X_OK);
}