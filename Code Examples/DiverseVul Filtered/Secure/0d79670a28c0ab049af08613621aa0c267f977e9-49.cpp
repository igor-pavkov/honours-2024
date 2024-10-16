static BOOL wf_cliprdr_array_ensure_capacity(wfClipboard* clipboard)
{
	if (!clipboard)
		return FALSE;

	if (clipboard->nFiles == clipboard->file_array_size)
	{
		size_t new_size;
		FILEDESCRIPTORW** new_fd;
		WCHAR** new_name;
		new_size = (clipboard->file_array_size + 1) * 2;
		new_fd = (FILEDESCRIPTORW**)realloc(clipboard->fileDescriptor,
		                                    new_size * sizeof(FILEDESCRIPTORW*));

		if (new_fd)
			clipboard->fileDescriptor = new_fd;

		new_name = (WCHAR**)realloc(clipboard->file_names, new_size * sizeof(WCHAR*));

		if (new_name)
			clipboard->file_names = new_name;

		if (!new_fd || !new_name)
			return FALSE;

		clipboard->file_array_size = new_size;
	}

	return TRUE;
}