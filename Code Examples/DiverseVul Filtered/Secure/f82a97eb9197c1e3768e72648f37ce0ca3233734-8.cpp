int mingw_open (const char *filename, int oflags, ...)
{
	va_list args;
	unsigned mode;
	int fd, create = (oflags & (O_CREAT | O_EXCL)) == (O_CREAT | O_EXCL);
	wchar_t wfilename[MAX_PATH];

	va_start(args, oflags);
	mode = va_arg(args, int);
	va_end(args);

	if (!is_valid_win32_path(filename)) {
		errno = create ? EINVAL : ENOENT;
		return -1;
	}

	if (filename && !strcmp(filename, "/dev/null"))
		filename = "nul";

	if (xutftowcs_path(wfilename, filename) < 0)
		return -1;
	fd = _wopen(wfilename, oflags, mode);

	if (fd < 0 && (oflags & O_ACCMODE) != O_RDONLY && errno == EACCES) {
		DWORD attrs = GetFileAttributesW(wfilename);
		if (attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY))
			errno = EISDIR;
	}
	if ((oflags & O_CREAT) && needs_hiding(filename)) {
		/*
		 * Internally, _wopen() uses the CreateFile() API which errors
		 * out with an ERROR_ACCESS_DENIED if CREATE_ALWAYS was
		 * specified and an already existing file's attributes do not
		 * match *exactly*. As there is no mode or flag we can set that
		 * would correspond to FILE_ATTRIBUTE_HIDDEN, let's just try
		 * again *without* the O_CREAT flag (that corresponds to the
		 * CREATE_ALWAYS flag of CreateFile()).
		 */
		if (fd < 0 && errno == EACCES)
			fd = _wopen(wfilename, oflags & ~O_CREAT, mode);
		if (fd >= 0 && set_hidden_flag(wfilename, 1))
			warning("could not mark '%s' as hidden.", filename);
	}
	return fd;
}