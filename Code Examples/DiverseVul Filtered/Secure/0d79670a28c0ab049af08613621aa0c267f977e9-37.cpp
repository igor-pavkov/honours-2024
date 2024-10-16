static BOOL cliprdr_GetUpdatedClipboardFormats(wfClipboard* clipboard, PUINT lpuiFormats,
                                               UINT cFormats, PUINT pcFormatsOut)
{
	UINT index = 0;
	UINT format = 0;
	BOOL clipboardOpen = FALSE;

	if (!clipboard->legacyApi)
		return clipboard->GetUpdatedClipboardFormats(lpuiFormats, cFormats, pcFormatsOut);

	clipboardOpen = try_open_clipboard(clipboard->hwnd);

	if (!clipboardOpen)
	{
		*pcFormatsOut = 0;
		return TRUE; /* Other app holding clipboard */
	}

	while (index < cFormats)
	{
		format = EnumClipboardFormats(format);

		if (!format)
			break;

		lpuiFormats[index] = format;
		index++;
	}

	*pcFormatsOut = index;
	CloseClipboard();
	return TRUE;
}