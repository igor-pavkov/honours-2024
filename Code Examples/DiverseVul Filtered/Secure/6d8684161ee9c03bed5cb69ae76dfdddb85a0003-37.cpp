FILE *mingw_freopen (const char *filename, const char *otype, FILE *stream)
{
	int hide = needs_hiding(filename);
	FILE *file;
	wchar_t wfilename[MAX_PATH], wotype[4];
	if (filename && !strcmp(filename, "/dev/null"))
		filename = "nul";
	if (xutftowcs_path(wfilename, filename) < 0 ||
		xutftowcs(wotype, otype, ARRAY_SIZE(wotype)) < 0)
		return NULL;
	if (hide && !access(filename, F_OK) && set_hidden_flag(wfilename, 0)) {
		error("could not unhide %s", filename);
		return NULL;
	}
	file = _wfreopen(wfilename, wotype, stream);
	if (file && hide && set_hidden_flag(wfilename, 1))
		warning("could not mark '%s' as hidden.", filename);
	return file;
}