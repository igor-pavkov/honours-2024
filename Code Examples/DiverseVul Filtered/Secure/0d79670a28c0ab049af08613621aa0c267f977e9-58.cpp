static FILEDESCRIPTORW* wf_cliprdr_get_file_descriptor(WCHAR* file_name, size_t pathLen)
{
	HANDLE hFile;
	FILEDESCRIPTORW* fd;
	fd = (FILEDESCRIPTORW*)calloc(1, sizeof(FILEDESCRIPTORW));

	if (!fd)
		return NULL;

	hFile = CreateFileW(file_name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
	                    FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		free(fd);
		return NULL;
	}

	fd->dwFlags = FD_ATTRIBUTES | FD_FILESIZE | FD_WRITESTIME | FD_PROGRESSUI;
	fd->dwFileAttributes = GetFileAttributes(file_name);

	if (!GetFileTime(hFile, NULL, NULL, &fd->ftLastWriteTime))
	{
		fd->dwFlags &= ~FD_WRITESTIME;
	}

	fd->nFileSizeLow = GetFileSize(hFile, &fd->nFileSizeHigh);
	wcscpy_s(fd->cFileName, sizeof(fd->cFileName) / 2, file_name + pathLen);
	CloseHandle(hFile);
	return fd;
}