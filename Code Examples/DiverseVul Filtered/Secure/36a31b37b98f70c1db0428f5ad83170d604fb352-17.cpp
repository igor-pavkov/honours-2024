vncProperties::Init(vncServer *server)
{
	// Save the server pointer
	m_server = server;

	// sf@2007 - Registry mode can still be forced for backward compatibility and OS version < Vista
	m_fUseRegistry = ((myIniFile.ReadInt("admin", "UseRegistry", 0) == 1) ? TRUE : FALSE);

	// Load the settings
	if (m_fUseRegistry)
		Load(TRUE);
	else
		LoadFromIniFile();

	// If the password is empty then always show a dialog
	char passwd[MAXPWLEN];
	m_server->GetPassword(passwd);
	{
	    vncPasswd::ToText plain(passwd, m_pref_Secure);
	    if (strlen(plain) == 0)
			 if (!m_allowproperties || !RunningAsAdministrator ()) {
				if(m_server->AuthRequired()) {
					MessageBoxSecure(NULL, sz_ID_NO_PASSWD_NO_OVERRIDE_ERR,
								sz_ID_WINVNC_ERROR,
								MB_OK | MB_ICONSTOP);
					PostQuitMessage(0);
				}
				/*else {
					if (!vncService::RunningAsService())
						MessageBoxSecure(NULL, sz_ID_NO_PASSWD_NO_OVERRIDE_WARN,
								sz_ID_WINVNC_ERROR,
								MB_OK | MB_ICONEXCLAMATION);
				}*/
			} else {
				// If null passwords are not allowed, ensure that one is entered!
				if (m_server->AuthRequired()) {
					char username[UNLEN+1];
					if (!vncService::CurrentUser(username, sizeof(username)))
						return FALSE;
					if (strcmp(username, "") == 0) {
						Lock_service_helper=true;
						MessageBoxSecure(NULL, sz_ID_NO_PASSWD_NO_LOGON_WARN,
									sz_ID_WINVNC_ERROR,
									MB_OK | MB_ICONEXCLAMATION);
						ShowAdmin(TRUE, FALSE);
						Lock_service_helper=false;
					} else {
						ShowAdmin(TRUE, TRUE);
					}
				}
			}
	}
	Lock_service_helper=false;
	return TRUE;
}