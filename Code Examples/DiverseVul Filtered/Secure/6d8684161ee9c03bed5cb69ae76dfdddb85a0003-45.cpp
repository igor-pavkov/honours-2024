int mingw_utime (const char *file_name, const struct utimbuf *times)
{
	FILETIME mft, aft;
	int fh, rc;
	DWORD attrs;
	wchar_t wfilename[MAX_PATH];
	if (xutftowcs_path(wfilename, file_name) < 0)
		return -1;

	/* must have write permission */
	attrs = GetFileAttributesW(wfilename);
	if (attrs != INVALID_FILE_ATTRIBUTES &&
	    (attrs & FILE_ATTRIBUTE_READONLY)) {
		/* ignore errors here; open() will report them */
		SetFileAttributesW(wfilename, attrs & ~FILE_ATTRIBUTE_READONLY);
	}

	if ((fh = _wopen(wfilename, O_RDWR | O_BINARY)) < 0) {
		rc = -1;
		goto revert_attrs;
	}

	if (times) {
		time_t_to_filetime(times->modtime, &mft);
		time_t_to_filetime(times->actime, &aft);
	} else {
		GetSystemTimeAsFileTime(&mft);
		aft = mft;
	}
	if (!SetFileTime((HANDLE)_get_osfhandle(fh), NULL, &aft, &mft)) {
		errno = EINVAL;
		rc = -1;
	} else
		rc = 0;
	close(fh);

revert_attrs:
	if (attrs != INVALID_FILE_ATTRIBUTES &&
	    (attrs & FILE_ATTRIBUTE_READONLY)) {
		/* ignore errors again */
		SetFileAttributesW(wfilename, attrs);
	}
	return rc;
}