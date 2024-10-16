static BOOL try_open_clipboard(HWND hwnd)
{
	size_t x;
	for (x = 0; x < 10; x++)
	{
		if (OpenClipboard(hwnd))
			return TRUE;
		Sleep(10);
	}
	return FALSE;
}