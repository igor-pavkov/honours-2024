static int has_valid_directory_prefix(wchar_t *wfilename)
{
	int n = wcslen(wfilename);

	while (n > 0) {
		wchar_t c = wfilename[--n];
		DWORD attributes;

		if (!is_dir_sep(c))
			continue;

		wfilename[n] = L'\0';
		attributes = GetFileAttributesW(wfilename);
		wfilename[n] = c;
		if (attributes == FILE_ATTRIBUTE_DIRECTORY ||
				attributes == FILE_ATTRIBUTE_DEVICE)
			return 1;
		if (attributes == INVALID_FILE_ATTRIBUTES)
			switch (GetLastError()) {
			case ERROR_PATH_NOT_FOUND:
				continue;
			case ERROR_FILE_NOT_FOUND:
				/* This implies parent directory exists. */
				return 1;
			}
		return 0;
	}
	return 1;
}