static void clear_file_array(wfClipboard* clipboard)
{
	size_t i;

	if (!clipboard)
		return;

	/* clear file_names array */
	if (clipboard->file_names)
	{
		for (i = 0; i < clipboard->nFiles; i++)
		{
			free(clipboard->file_names[i]);
			clipboard->file_names[i] = NULL;
		}

		free(clipboard->file_names);
		clipboard->file_names = NULL;
	}

	/* clear fileDescriptor array */
	if (clipboard->fileDescriptor)
	{
		for (i = 0; i < clipboard->nFiles; i++)
		{
			free(clipboard->fileDescriptor[i]);
			clipboard->fileDescriptor[i] = NULL;
		}

		free(clipboard->fileDescriptor);
		clipboard->fileDescriptor = NULL;
	}

	clipboard->file_array_size = 0;
	clipboard->nFiles = 0;
}