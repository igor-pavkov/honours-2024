static BOOL wf_cliprdr_get_file_contents(WCHAR* file_name, BYTE* buffer, LONG positionLow,
                                         LONG positionHigh, DWORD nRequested, DWORD* puSize)
{
	BOOL res = FALSE;
	HANDLE hFile;
	DWORD nGet, rc;

	if (!file_name || !buffer || !puSize)
	{
		WLog_ERR(TAG, "get file contents Invalid Arguments.");
		return FALSE;
	}

	hFile = CreateFileW(file_name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
	                    FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	rc = SetFilePointer(hFile, positionLow, &positionHigh, FILE_BEGIN);

	if (rc == INVALID_SET_FILE_POINTER)
		goto error;

	if (!ReadFile(hFile, buffer, nRequested, &nGet, NULL))
	{
		DEBUG_CLIPRDR("ReadFile failed with 0x%08lX.", GetLastError());
		goto error;
	}

	res = TRUE;
error:

	if (!CloseHandle(hFile))
		res = FALSE;

	if (res)
		*puSize = nGet;

	return res;
}