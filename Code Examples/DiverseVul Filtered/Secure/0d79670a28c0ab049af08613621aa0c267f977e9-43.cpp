static UINT wf_cliprdr_server_format_list(CliprdrClientContext* context,
                                          const CLIPRDR_FORMAT_LIST* formatList)
{
	UINT rc = ERROR_INTERNAL_ERROR;
	UINT32 i;
	formatMapping* mapping;
	CLIPRDR_FORMAT* format;
	wfClipboard* clipboard = (wfClipboard*)context->custom;

	if (!clear_format_map(clipboard))
		return ERROR_INTERNAL_ERROR;

	for (i = 0; i < formatList->numFormats; i++)
	{
		format = &(formatList->formats[i]);
		mapping = &(clipboard->format_mappings[i]);
		mapping->remote_format_id = format->formatId;

		if (format->formatName)
		{
			int size = MultiByteToWideChar(CP_UTF8, 0, format->formatName,
			                               strlen(format->formatName), NULL, 0);
			mapping->name = calloc(size + 1, sizeof(WCHAR));

			if (mapping->name)
			{
				MultiByteToWideChar(CP_UTF8, 0, format->formatName, strlen(format->formatName),
				                    mapping->name, size);
				mapping->local_format_id = RegisterClipboardFormatW((LPWSTR)mapping->name);
			}
		}
		else
		{
			mapping->name = NULL;
			mapping->local_format_id = mapping->remote_format_id;
		}

		clipboard->map_size++;
		map_ensure_capacity(clipboard);
	}

	if (file_transferring(clipboard))
	{
		if (PostMessage(clipboard->hwnd, WM_CLIPRDR_MESSAGE, OLE_SETCLIPBOARD, 0))
			rc = CHANNEL_RC_OK;
	}
	else
	{
		if (!try_open_clipboard(clipboard->hwnd))
			return CHANNEL_RC_OK; /* Ignore, other app holding clipboard */

		if (EmptyClipboard())
		{
			for (i = 0; i < (UINT32)clipboard->map_size; i++)
				SetClipboardData(clipboard->format_mappings[i].local_format_id, NULL);

			rc = CHANNEL_RC_OK;
		}

		if (!CloseClipboard() && GetLastError())
			return ERROR_INTERNAL_ERROR;
	}

	return rc;
}