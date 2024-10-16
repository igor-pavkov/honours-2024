void vncProperties::ExpandBox(HWND hDlg, BOOL fExpand)
{
	// if the dialog is already in the requested state, return
	// immediately.
	if (fExpand == m_bExpanded) return;

	RECT rcWnd, rcDefaultBox, rcChild, rcIntersection;
	HWND wndChild = NULL;
	HWND wndDefaultBox = NULL;

	// get the window of the button 
	HWND  pCtrl = GetDlgItem(hDlg, IDC_SHOWOPTIONS);
	if (pCtrl == NULL) return;

	wndDefaultBox = GetDlgItem(hDlg, IDC_DEFAULTBOX);
	if (wndDefaultBox == NULL) return;

	if (!fExpand) SendMessage(GetDlgItem(hDlg, IDC_BUTTON_EXPAND), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hBmpExpand);
	else SendMessage(GetDlgItem(hDlg, IDC_BUTTON_EXPAND), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hBmpCollaps);
	// retrieve coordinates for the default child window
	GetWindowRect(wndDefaultBox, &rcDefaultBox);

	// enable/disable all of the child window outside of the default box.
	wndChild = GetTopWindow(hDlg);

	for (; wndChild != NULL; wndChild = GetWindow(wndChild, GW_HWNDNEXT))
	{
		// get rectangle occupied by child window in screen coordinates.
		GetWindowRect(wndChild, &rcChild);

		if (!IntersectRect(&rcIntersection, &rcChild, &rcDefaultBox))
		{
			EnableWindow(wndChild, fExpand);
		}
	}

	if (!fExpand)  // we are contracting
	{
		_ASSERT(m_bExpanded);
		GetWindowRect(hDlg, &rcWnd);

		// this is the first time we are being called to shrink the dialog
		// box.  The dialog box is currently in its expanded size and we must
		// save the expanded width and height so that it can be restored
		// later when the dialog box is expanded.

		if (cx == 0 && cy == 0)
		{
			cx = rcDefaultBox.right - rcWnd.left;
			cy = rcWnd.bottom - rcWnd.top;

			// we also hide the default box here so that it is not visible
			ShowWindow(wndDefaultBox, SW_HIDE);
		}


		// shrink the dialog box so that it encompasses everything from the top,
		// left up to and including the default box.
		SetWindowPos(hDlg, NULL, 0, 0,
			rcDefaultBox.right - rcWnd.left,
			rcDefaultBox.bottom - rcWnd.top,
			SWP_NOZORDER | SWP_NOMOVE);

		SetWindowText(pCtrl, "Advanced options");

		// record that the dialog is contracted.
		m_bExpanded = FALSE;
	}
	else // we are expanding
	{
		_ASSERT(!m_bExpanded);
		SetWindowPos(hDlg, NULL, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOMOVE);

		// make sure that the entire dialog box is visible on the user's
		// screen.
		SendMessage(hDlg, DM_REPOSITION, 0, 0);
		SetWindowText(pCtrl, "Hide");
		m_bExpanded = TRUE;
	}
}