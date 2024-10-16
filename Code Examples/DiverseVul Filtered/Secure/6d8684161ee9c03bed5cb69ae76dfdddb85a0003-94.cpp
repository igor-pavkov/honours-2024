char *mingw_getcwd(char *pointer, int len)
{
	wchar_t wpointer[MAX_PATH];
	if (!_wgetcwd(wpointer, ARRAY_SIZE(wpointer)))
		return NULL;
	if (xwcstoutf(pointer, wpointer, len) < 0)
		return NULL;
	convert_slashes(pointer);
	return pointer;
}