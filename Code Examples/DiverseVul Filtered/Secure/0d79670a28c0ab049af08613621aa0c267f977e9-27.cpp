static LRESULT CALLBACK cliprdr_proc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	static wfClipboard* clipboard = NULL;

	switch (Msg)
	{
		case WM_CREATE:
			DEBUG_CLIPRDR("info: WM_CREATE");
			clipboard = (wfClipboard*)((CREATESTRUCT*)lParam)->lpCreateParams;
			clipboard->hwnd = hWnd;

			if (!clipboard->legacyApi)
				clipboard->AddClipboardFormatListener(hWnd);
			else
				clipboard->hWndNextViewer = SetClipboardViewer(hWnd);

			break;

		case WM_CLOSE:
			DEBUG_CLIPRDR("info: WM_CLOSE");

			if (!clipboard->legacyApi)
				clipboard->RemoveClipboardFormatListener(hWnd);

			break;

		case WM_DESTROY:
			if (clipboard->legacyApi)
				ChangeClipboardChain(hWnd, clipboard->hWndNextViewer);

			break;

		case WM_CLIPBOARDUPDATE:
			DEBUG_CLIPRDR("info: WM_CLIPBOARDUPDATE");

			if (clipboard->sync)
			{
				if ((GetClipboardOwner() != clipboard->hwnd) &&
				    (S_FALSE == OleIsCurrentClipboard(clipboard->data_obj)))
				{
					if (clipboard->hmem)
					{
						GlobalFree(clipboard->hmem);
						clipboard->hmem = NULL;
					}

					cliprdr_send_format_list(clipboard);
				}
			}

			break;

		case WM_RENDERALLFORMATS:
			DEBUG_CLIPRDR("info: WM_RENDERALLFORMATS");

			/* discard all contexts in clipboard */
			if (!try_open_clipboard(clipboard->hwnd))
			{
				DEBUG_CLIPRDR("OpenClipboard failed with 0x%x", GetLastError());
				break;
			}

			EmptyClipboard();
			CloseClipboard();
			break;

		case WM_RENDERFORMAT:
			DEBUG_CLIPRDR("info: WM_RENDERFORMAT");

			if (cliprdr_send_data_request(clipboard, (UINT32)wParam) != 0)
			{
				DEBUG_CLIPRDR("error: cliprdr_send_data_request failed.");
				break;
			}

			if (!SetClipboardData((UINT)wParam, clipboard->hmem))
			{
				DEBUG_CLIPRDR("SetClipboardData failed with 0x%x", GetLastError());

				if (clipboard->hmem)
				{
					GlobalFree(clipboard->hmem);
					clipboard->hmem = NULL;
				}
			}

			/* Note: GlobalFree() is not needed when success */
			break;

		case WM_DRAWCLIPBOARD:
			if (clipboard->legacyApi)
			{
				if ((GetClipboardOwner() != clipboard->hwnd) &&
				    (S_FALSE == OleIsCurrentClipboard(clipboard->data_obj)))
				{
					cliprdr_send_format_list(clipboard);
				}

				SendMessage(clipboard->hWndNextViewer, Msg, wParam, lParam);
			}

			break;

		case WM_CHANGECBCHAIN:
			if (clipboard->legacyApi)
			{
				HWND hWndCurrViewer = (HWND)wParam;
				HWND hWndNextViewer = (HWND)lParam;

				if (hWndCurrViewer == clipboard->hWndNextViewer)
					clipboard->hWndNextViewer = hWndNextViewer;
				else if (clipboard->hWndNextViewer)
					SendMessage(clipboard->hWndNextViewer, Msg, wParam, lParam);
			}

			break;

		case WM_CLIPRDR_MESSAGE:
			DEBUG_CLIPRDR("info: WM_CLIPRDR_MESSAGE");

			switch (wParam)
			{
				case OLE_SETCLIPBOARD:
					DEBUG_CLIPRDR("info: OLE_SETCLIPBOARD");

					if (S_FALSE == OleIsCurrentClipboard(clipboard->data_obj))
					{
						if (wf_create_file_obj(clipboard, &clipboard->data_obj))
						{
							if (OleSetClipboard(clipboard->data_obj) != S_OK)
							{
								wf_destroy_file_obj(clipboard->data_obj);
								clipboard->data_obj = NULL;
							}
						}
					}

					break;

				default:
					break;
			}

			break;

		case WM_DESTROYCLIPBOARD:
		case WM_ASKCBFORMATNAME:
		case WM_HSCROLLCLIPBOARD:
		case WM_PAINTCLIPBOARD:
		case WM_SIZECLIPBOARD:
		case WM_VSCROLLCLIPBOARD:
		default:
			return DefWindowProc(hWnd, Msg, wParam, lParam);
	}

	return 0;
}