static BOOL wf_cliprdr_process_filename(wfClipboard* clipboard, WCHAR* wFileName, size_t str_len)
{
	size_t pathLen;
	size_t offset = str_len;

	if (!clipboard || !wFileName)
		return FALSE;

	/* find the last '\' in full file name */
	while (offset > 0)
	{
		if (wFileName[offset] == L'\\')
			break;
		else
			offset--;
	}

	pathLen = offset + 1;

	if (!wf_cliprdr_add_to_file_arrays(clipboard, wFileName, pathLen))
		return FALSE;

	if ((clipboard->fileDescriptor[clipboard->nFiles - 1]->dwFileAttributes &
	     FILE_ATTRIBUTE_DIRECTORY) != 0)
	{
		/* this is a directory */
		if (!wf_cliprdr_traverse_directory(clipboard, wFileName, pathLen))
			return FALSE;
	}

	return TRUE;
}