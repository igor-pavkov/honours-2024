static int is_dir_empty(const wchar_t *wpath)
{
	WIN32_FIND_DATAW findbuf;
	HANDLE handle;
	wchar_t wbuf[MAX_PATH + 2];
	wcscpy(wbuf, wpath);
	wcscat(wbuf, L"\\*");
	handle = FindFirstFileW(wbuf, &findbuf);
	if (handle == INVALID_HANDLE_VALUE)
		return GetLastError() == ERROR_NO_MORE_FILES;

	while (!wcscmp(findbuf.cFileName, L".") ||
			!wcscmp(findbuf.cFileName, L".."))
		if (!FindNextFileW(handle, &findbuf)) {
			DWORD err = GetLastError();
			FindClose(handle);
			return err == ERROR_NO_MORE_FILES;
		}
	FindClose(handle);
	return 0;
}