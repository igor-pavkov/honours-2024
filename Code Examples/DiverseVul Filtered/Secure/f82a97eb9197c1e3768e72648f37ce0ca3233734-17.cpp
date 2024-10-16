int mingw_mkdir(const char *path, int mode)
{
	int ret;
	wchar_t wpath[MAX_PATH];

	if (!is_valid_win32_path(path)) {
		errno = EINVAL;
		return -1;
	}

	if (xutftowcs_path(wpath, path) < 0)
		return -1;
	ret = _wmkdir(wpath);
	if (!ret && needs_hiding(path))
		return set_hidden_flag(wpath, 1);
	return ret;
}