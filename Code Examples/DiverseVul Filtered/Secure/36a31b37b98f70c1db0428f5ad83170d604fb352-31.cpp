vncProperties::InitPortSettings(HWND hwnd)
{
	BOOL bConnectSock = m_server->SockConnected();
	BOOL bAutoPort = m_server->AutoPortSelect();
	UINT port_rfb = m_server->GetPort();
	UINT port_http = m_server->GetHttpPort();
	int d1 = PORT_TO_DISPLAY(port_rfb);
	int d2 = HPORT_TO_DISPLAY(port_http);
	BOOL bValidDisplay = (d1 == d2 && d1 >= 0 && d1 <= 99);

	CheckDlgButton(hwnd, IDC_PORTNO_AUTO,
		(bAutoPort) ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_SPECDISPLAY,
		(!bAutoPort && bValidDisplay) ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_SPECPORT,
		(!bAutoPort && !bValidDisplay) ? BST_CHECKED : BST_UNCHECKED);

	EnableWindow(GetDlgItem(hwnd, IDC_PORTNO_AUTO), bConnectSock);
	EnableWindow(GetDlgItem(hwnd, IDC_SPECDISPLAY), bConnectSock);
	EnableWindow(GetDlgItem(hwnd, IDC_SPECPORT), bConnectSock);

	if (bValidDisplay) {
		SetDlgItemInt(hwnd, IDC_DISPLAYNO, d1, FALSE);
	} else {
		SetDlgItemText(hwnd, IDC_DISPLAYNO, "");
	}
	SetDlgItemInt(hwnd, IDC_PORTRFB, port_rfb, FALSE);
	SetDlgItemInt(hwnd, IDC_PORTHTTP, port_http, FALSE);

	EnableWindow(GetDlgItem(hwnd, IDC_DISPLAYNO),
		bConnectSock && !bAutoPort && bValidDisplay);
	EnableWindow(GetDlgItem(hwnd, IDC_PORTRFB),
		bConnectSock && !bAutoPort && !bValidDisplay);
	EnableWindow(GetDlgItem(hwnd, IDC_PORTHTTP),
		bConnectSock && !bAutoPort && !bValidDisplay);
}