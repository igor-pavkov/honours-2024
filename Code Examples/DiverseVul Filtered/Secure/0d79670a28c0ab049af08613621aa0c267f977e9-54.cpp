static DWORD WINAPI cliprdr_thread_func(LPVOID arg)
{
	int ret;
	MSG msg;
	BOOL mcode;
	wfClipboard* clipboard = (wfClipboard*)arg;
	OleInitialize(0);

	if ((ret = create_cliprdr_window(clipboard)) != 0)
	{
		OleUninitialize();
		DEBUG_CLIPRDR("error: create clipboard window failed.");
		return 0;
	}

	while ((mcode = GetMessage(&msg, 0, 0, 0)) != 0)
	{
		if (mcode == -1)
		{
			DEBUG_CLIPRDR("error: clipboard thread GetMessage failed.");
			break;
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	OleUninitialize();
	return 0;
}