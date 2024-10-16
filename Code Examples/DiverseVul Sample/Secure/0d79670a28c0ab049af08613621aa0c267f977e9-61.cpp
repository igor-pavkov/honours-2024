static UINT cliprdr_send_format_list(wfClipboard* clipboard)
{
	UINT rc;
	int count = 0;
	UINT32 index;
	UINT32 numFormats = 0;
	UINT32 formatId = 0;
	char formatName[1024];
	CLIPRDR_FORMAT* formats = NULL;
	CLIPRDR_FORMAT_LIST formatList = { 0 };

	if (!clipboard)
		return ERROR_INTERNAL_ERROR;

	ZeroMemory(&formatList, sizeof(CLIPRDR_FORMAT_LIST));

	/* Ignore if other app is holding clipboard */
	if (try_open_clipboard(clipboard->hwnd))
	{
		count = CountClipboardFormats();
		numFormats = (UINT32)count;
		formats = (CLIPRDR_FORMAT*)calloc(numFormats, sizeof(CLIPRDR_FORMAT));

		if (!formats)
		{
			CloseClipboard();
			return CHANNEL_RC_NO_MEMORY;
		}

		index = 0;

		if (IsClipboardFormatAvailable(CF_HDROP))
		{
			formats[index++].formatId = RegisterClipboardFormat(CFSTR_FILEDESCRIPTORW);
			formats[index++].formatId = RegisterClipboardFormat(CFSTR_FILECONTENTS);
		}
		else
		{
			while (formatId = EnumClipboardFormats(formatId))
				formats[index++].formatId = formatId;
		}

		numFormats = index;

		if (!CloseClipboard())
		{
			free(formats);
			return ERROR_INTERNAL_ERROR;
		}

		for (index = 0; index < numFormats; index++)
		{
			if (GetClipboardFormatNameA(formats[index].formatId, formatName, sizeof(formatName)))
			{
				formats[index].formatName = _strdup(formatName);
			}
		}
	}

	formatList.numFormats = numFormats;
	formatList.formats = formats;
	formatList.msgType = CB_FORMAT_LIST;
	rc = clipboard->context->ClientFormatList(clipboard->context, &formatList);

	for (index = 0; index < numFormats; index++)
		free(formats[index].formatName);

	free(formats);
	return rc;
}