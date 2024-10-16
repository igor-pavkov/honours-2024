int mingw_rename(const char *pold, const char *pnew)
{
	DWORD attrs, gle;
	int tries = 0;
	wchar_t wpold[MAX_PATH], wpnew[MAX_PATH];
	if (xutftowcs_path(wpold, pold) < 0 || xutftowcs_path(wpnew, pnew) < 0)
		return -1;

	/*
	 * Try native rename() first to get errno right.
	 * It is based on MoveFile(), which cannot overwrite existing files.
	 */
	if (!_wrename(wpold, wpnew))
		return 0;
	if (errno != EEXIST)
		return -1;
repeat:
	if (MoveFileExW(wpold, wpnew, MOVEFILE_REPLACE_EXISTING))
		return 0;
	/* TODO: translate more errors */
	gle = GetLastError();
	if (gle == ERROR_ACCESS_DENIED &&
	    (attrs = GetFileAttributesW(wpnew)) != INVALID_FILE_ATTRIBUTES) {
		if (attrs & FILE_ATTRIBUTE_DIRECTORY) {
			DWORD attrsold = GetFileAttributesW(wpold);
			if (attrsold == INVALID_FILE_ATTRIBUTES ||
			    !(attrsold & FILE_ATTRIBUTE_DIRECTORY))
				errno = EISDIR;
			else if (!_wrmdir(wpnew))
				goto repeat;
			return -1;
		}
		if ((attrs & FILE_ATTRIBUTE_READONLY) &&
		    SetFileAttributesW(wpnew, attrs & ~FILE_ATTRIBUTE_READONLY)) {
			if (MoveFileExW(wpold, wpnew, MOVEFILE_REPLACE_EXISTING))
				return 0;
			gle = GetLastError();
			/* revert file attributes on failure */
			SetFileAttributesW(wpnew, attrs);
		}
	}
	if (tries < ARRAY_SIZE(delay) && gle == ERROR_ACCESS_DENIED) {
		/*
		 * We assume that some other process had the source or
		 * destination file open at the wrong moment and retry.
		 * In order to give the other process a higher chance to
		 * complete its operation, we give up our time slice now.
		 * If we have to retry again, we do sleep a bit.
		 */
		Sleep(delay[tries]);
		tries++;
		goto repeat;
	}
	if (gle == ERROR_ACCESS_DENIED &&
	       ask_yes_no_if_possible("Rename from '%s' to '%s' failed. "
		       "Should I try again?", pold, pnew))
		goto repeat;

	errno = EACCES;
	return -1;
}