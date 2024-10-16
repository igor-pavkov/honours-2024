static BOOL wf_cliprdr_add_to_file_arrays(wfClipboard* clipboard, WCHAR* full_file_name,
                                          size_t pathLen)
{
	if (!wf_cliprdr_array_ensure_capacity(clipboard))
		return FALSE;

	/* add to name array */
	clipboard->file_names[clipboard->nFiles] = (LPWSTR)malloc(MAX_PATH * 2);

	if (!clipboard->file_names[clipboard->nFiles])
		return FALSE;

	wcscpy_s(clipboard->file_names[clipboard->nFiles], MAX_PATH, full_file_name);
	/* add to descriptor array */
	clipboard->fileDescriptor[clipboard->nFiles] =
	    wf_cliprdr_get_file_descriptor(full_file_name, pathLen);

	if (!clipboard->fileDescriptor[clipboard->nFiles])
	{
		free(clipboard->file_names[clipboard->nFiles]);
		return FALSE;
	}

	clipboard->nFiles++;
	return TRUE;
}