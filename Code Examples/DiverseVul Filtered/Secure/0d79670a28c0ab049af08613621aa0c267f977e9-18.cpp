static int create_cliprdr_window(wfClipboard* clipboard)
{
	WNDCLASSEX wnd_cls;
	ZeroMemory(&wnd_cls, sizeof(WNDCLASSEX));
	wnd_cls.cbSize = sizeof(WNDCLASSEX);
	wnd_cls.style = CS_OWNDC;
	wnd_cls.lpfnWndProc = cliprdr_proc;
	wnd_cls.cbClsExtra = 0;
	wnd_cls.cbWndExtra = 0;
	wnd_cls.hIcon = NULL;
	wnd_cls.hCursor = NULL;
	wnd_cls.hbrBackground = NULL;
	wnd_cls.lpszMenuName = NULL;
	wnd_cls.lpszClassName = _T("ClipboardHiddenMessageProcessor");
	wnd_cls.hInstance = GetModuleHandle(NULL);
	wnd_cls.hIconSm = NULL;
	RegisterClassEx(&wnd_cls);
	clipboard->hwnd =
	    CreateWindowEx(WS_EX_LEFT, _T("ClipboardHiddenMessageProcessor"), _T("rdpclip"), 0, 0, 0, 0,
	                   0, HWND_MESSAGE, NULL, GetModuleHandle(NULL), clipboard);

	if (!clipboard->hwnd)
	{
		DEBUG_CLIPRDR("error: CreateWindowEx failed with %x.", GetLastError());
		return -1;
	}

	return 0;
}