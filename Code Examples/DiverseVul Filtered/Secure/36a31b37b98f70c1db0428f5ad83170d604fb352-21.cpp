void Secure_Plugin(char *szPlugin)
{
	CDSMPlugin* m_pDSMPlugin = NULL;
	m_pDSMPlugin = new CDSMPlugin();
	m_pDSMPlugin->LoadPlugin(szPlugin, false);
	if (m_pDSMPlugin->IsLoaded())
	{
		char szParams[32];
		strcpy_s(szParams, "NoPassword,");
		strcat_s(szParams, "server-app");

		HDESK desktop;
		desktop = OpenInputDesktop(0, FALSE,
			DESKTOP_CREATEMENU | DESKTOP_CREATEWINDOW |
			DESKTOP_ENUMERATE | DESKTOP_HOOKCONTROL |
			DESKTOP_WRITEOBJECTS | DESKTOP_READOBJECTS |
			DESKTOP_SWITCHDESKTOP | GENERIC_WRITE
			);

		if (desktop == NULL)
			vnclog.Print(LL_INTERR, VNCLOG("OpenInputdesktop Error \n"));
		else
			vnclog.Print(LL_INTERR, VNCLOG("OpenInputdesktop OK\n"));

		HDESK old_desktop = GetThreadDesktop(GetCurrentThreadId());
		DWORD dummy;

		char new_name[256];
		if (desktop)
		{
			if (!GetUserObjectInformation(desktop, UOI_NAME, &new_name, 256, &dummy))
			{
				vnclog.Print(LL_INTERR, VNCLOG("!GetUserObjectInformation \n"));
			}

			vnclog.Print(LL_INTERR, VNCLOG("SelectHDESK to %s (%x) from %x\n"), new_name, desktop, old_desktop);

			if (!SetThreadDesktop(desktop))
			{
				vnclog.Print(LL_INTERR, VNCLOG("SelectHDESK:!SetThreadDesktop \n"));
			}
		}

		HRESULT hr = CoInitialize(NULL);
		HWND hwnd2 = CreateWindowA("STATIC", "dummy", WS_VISIBLE, 0, 0, 100, 100, NULL, NULL, NULL, NULL);
		ShowWindow(hwnd2, SW_HIDE);
		char* szNewConfig = NULL;
		char DSMPluginConfig[512];
		DSMPluginConfig[0] = '\0';
		IniFile myIniFile;
		myIniFile.ReadString("admin", "DSMPluginConfig", DSMPluginConfig, 512);
		m_pDSMPlugin->SetPluginParams(hwnd2, szParams, DSMPluginConfig, &szNewConfig);


		if (szNewConfig != NULL && strlen(szNewConfig) > 0) {
			strcpy_s(DSMPluginConfig, 511, szNewConfig);
		}
		myIniFile.WriteString("admin", "DSMPluginConfig", DSMPluginConfig);


		CoUninitialize();
		SetThreadDesktop(old_desktop);
		if (desktop) CloseDesktop(desktop);
	}
	if (m_pDSMPlugin != NULL) delete(m_pDSMPlugin);
}