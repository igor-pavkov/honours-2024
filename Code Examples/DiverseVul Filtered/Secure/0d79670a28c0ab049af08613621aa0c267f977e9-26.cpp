static BOOL wf_cliprdr_traverse_directory(wfClipboard* clipboard, WCHAR* Dir, size_t pathLen)
{
	HANDLE hFind;
	WCHAR DirSpec[MAX_PATH];
	WIN32_FIND_DATA FindFileData;

	if (!clipboard || !Dir)
		return FALSE;

	StringCchCopy(DirSpec, MAX_PATH, Dir);
	StringCchCat(DirSpec, MAX_PATH, TEXT("\\*"));
	hFind = FindFirstFile(DirSpec, &FindFileData);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		DEBUG_CLIPRDR("FindFirstFile failed with 0x%x.", GetLastError());
		return FALSE;
	}

	while (FindNextFile(hFind, &FindFileData))
	{
		if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 &&
		        wcscmp(FindFileData.cFileName, _T(".")) == 0 ||
		    wcscmp(FindFileData.cFileName, _T("..")) == 0)
		{
			continue;
		}

		if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
		{
			WCHAR DirAdd[MAX_PATH];
			StringCchCopy(DirAdd, MAX_PATH, Dir);
			StringCchCat(DirAdd, MAX_PATH, _T("\\"));
			StringCchCat(DirAdd, MAX_PATH, FindFileData.cFileName);

			if (!wf_cliprdr_add_to_file_arrays(clipboard, DirAdd, pathLen))
				return FALSE;

			if (!wf_cliprdr_traverse_directory(clipboard, DirAdd, pathLen))
				return FALSE;
		}
		else
		{
			WCHAR fileName[MAX_PATH];
			StringCchCopy(fileName, MAX_PATH, Dir);
			StringCchCat(fileName, MAX_PATH, _T("\\"));
			StringCchCat(fileName, MAX_PATH, FindFileData.cFileName);

			if (!wf_cliprdr_add_to_file_arrays(clipboard, fileName, pathLen))
				return FALSE;
		}
	}

	FindClose(hFind);
	return TRUE;
}