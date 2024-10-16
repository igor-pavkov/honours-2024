vncProperties::Load(BOOL usersettings)
{
	vnclog.Print(LL_INTINFO, VNCLOG("***** DBG - Entering Load\n"));

	//if (m_dlgvisible) {
	//	vnclog.Print(LL_INTWARN, VNCLOG("service helper invoked while Properties panel displayed\n"));
	//	return;
	//}
	ResetRegistry();

	if (vncService::RunningAsService()) usersettings=false;

	// sf@2007 - Vista mode
	// The WinVNC service mode is not used under Vista (due to Session0 isolation)
	// Default settings (Service mode) are used when WinVNC app in run under Vista login screen
	// User settings (loggued user mode) are used when WinVNC app in run in a user session
	// Todo: Maybe we should additionally check OS version...
	if (m_server->RunningFromExternalService())
		usersettings=false;

	m_usersettings = usersettings;

	if (m_usersettings)
		vnclog.Print(LL_INTINFO, VNCLOG("***** DBG - User mode\n"));
	else
		vnclog.Print(LL_INTINFO, VNCLOG("***** DBG - Service mode\n"));
	
	char username[UNLEN+1];
	HKEY hkLocal, hkLocalUser, hkDefault;
	DWORD dw;
	
	// NEW (R3) PREFERENCES ALGORITHM
	// 1.	Look in HKEY_LOCAL_MACHINE/Software/ORL/WinVNC3/%username%
	//		for sysadmin-defined, user-specific settings.
	// 2.	If not found, fall back to %username%=Default
	// 3.	If AllowOverrides is set then load settings from
	//		HKEY_CURRENT_USER/Software/ORL/WinVNC3

	// GET THE CORRECT KEY TO READ FROM

	// Get the user name / service name
	if (!vncService::CurrentUser((char *)&username, sizeof(username)))
	{
		vnclog.Print(LL_INTINFO, VNCLOG("***** DBG - NO current user\n"));
		return;
	}

	// If there is no user logged on them default to SYSTEM
	if (strcmp(username, "") == 0)
	{
		vnclog.Print(LL_INTINFO, VNCLOG("***** DBG - Force USER SYSTEM 1\n"));
		strcpy_s((char *)&username, UNLEN+1, "SYSTEM");
	}


	vnclog.Print(LL_INTINFO, VNCLOG("***** DBG - UserName = %s\n"), username);

	// Try to get the machine registry key for WinVNC
	if (RegCreateKeyEx(HKEY_LOCAL_MACHINE,
		WINVNC_REGISTRY_KEY,
		0, REG_NONE, REG_OPTION_NON_VOLATILE,
		KEY_READ, NULL, &hkLocal, &dw) != ERROR_SUCCESS)
		{
		hkLocalUser=NULL;
		hkDefault=NULL;
		goto LABELUSERSETTINGS;
		}

	// Now try to get the per-user local key
	if (RegOpenKeyEx(hkLocal,
		username,
		0, KEY_READ,
		&hkLocalUser) != ERROR_SUCCESS)
		hkLocalUser = NULL;

	// Get the default key
	if (RegCreateKeyEx(hkLocal,
		"Default",
		0, REG_NONE, REG_OPTION_NON_VOLATILE,
		KEY_READ,
		NULL,
		&hkDefault,
		&dw) != ERROR_SUCCESS)
		hkDefault = NULL;

	// LOAD THE MACHINE-LEVEL PREFS

	vnclog.Print(LL_INTINFO, VNCLOG("***** DBG - Machine level prefs\n"));

	// Logging/debugging prefs
	vnclog.Print(LL_INTINFO, VNCLOG("loading local-only settings\n"));
	//vnclog.SetMode(LoadInt(hkLocal, "DebugMode", 0));
	//vnclog.SetLevel(LoadInt(hkLocal, "DebugLevel", 0));

	// Disable Tray Icon
	m_server->SetDisableTrayIcon(LoadInt(hkLocal, "DisableTrayIcon", false));
	m_server->SetRdpmode(LoadInt(hkLocal, "rdpmode", 0));
	m_server->SetNoScreensaver(LoadInt(hkLocal, "noscreensaver", 0));

	// Authentication required, loopback allowed, loopbackOnly

	m_server->SetLoopbackOnly(LoadInt(hkLocal, "LoopbackOnly", false));

	m_pref_Secure = false;
	m_pref_Secure = LoadInt(hkLocal, "Secure", m_pref_Secure);
	m_server->Secure(m_pref_Secure);

	m_pref_RequireMSLogon=false;
	m_pref_RequireMSLogon = LoadInt(hkLocal, "MSLogonRequired", m_pref_RequireMSLogon);
	m_server->RequireMSLogon(m_pref_RequireMSLogon);

	// Marscha@2004 - authSSP: added NewMSLogon checkbox to admin props page
	m_pref_NewMSLogon = false;
	m_pref_NewMSLogon = LoadInt(hkLocal, "NewMSLogon", m_pref_NewMSLogon);
	m_server->SetNewMSLogon(m_pref_NewMSLogon);

	m_pref_ReverseAuthRequired = true;
	m_pref_ReverseAuthRequired = LoadInt(hkLocal, "ReverseAuthRequired", m_pref_ReverseAuthRequired);
	m_server->SetReverseAuthRequired(m_pref_ReverseAuthRequired);

	// sf@2003 - Moved DSM params here
	m_pref_UseDSMPlugin=false;
	m_pref_UseDSMPlugin = LoadInt(hkLocal, "UseDSMPlugin", m_pref_UseDSMPlugin);
	LoadDSMPluginName(hkLocal, m_pref_szDSMPlugin);	
	
	//adzm 2010-05-12 - dsmplugin config
	{
		char* szBuffer = LoadString(hkLocal, "DSMPluginConfig");
		if (szBuffer) {
			strncpy_s(m_pref_DSMPluginConfig, sizeof(m_pref_DSMPluginConfig) - 1, szBuffer, _TRUNCATE);
			delete[] szBuffer;
		} else {
			m_pref_DSMPluginConfig[0] = '\0';
		}
	}
#ifdef IPV6V4
	m_server->SetIPV6(LoadInt(hkLocal, "UseIpv6", true));
#endif
	if (m_server->LoopbackOnly()) m_server->SetLoopbackOk(true);
	else m_server->SetLoopbackOk(LoadInt(hkLocal, "AllowLoopback", true));
	m_server->SetAuthRequired(LoadInt(hkLocal, "AuthRequired", true));

	m_server->SetConnectPriority(LoadInt(hkLocal, "ConnectPriority", 0));
	if (!m_server->LoopbackOnly())
	{
		char *authhosts = LoadString(hkLocal, "AuthHosts");
		if (authhosts != 0) {
			m_server->SetAuthHosts(authhosts);
			delete [] authhosts;
		} else {
			m_server->SetAuthHosts(0);
		}
	} else {
		m_server->SetAuthHosts(0);
	}

	// If Socket connections are allowed, should the HTTP server be enabled?
LABELUSERSETTINGS:
	// LOAD THE USER PREFERENCES
	//vnclog.Print(LL_INTINFO, VNCLOG("***** DBG - Load User Preferences\n"));

	// Set the default user prefs
	vnclog.Print(LL_INTINFO, VNCLOG("clearing user settings\n"));
	m_pref_AutoPortSelect=TRUE;
    m_pref_HTTPConnect = TRUE;
	m_pref_PortNumber = RFB_PORT_OFFSET; 
	m_pref_SockConnect=TRUE;
	{
	    vncPasswd::FromClear crypt(m_pref_Secure);
	    memcpy(m_pref_passwd, crypt, MAXPWLEN);
	}
	m_pref_QuerySetting=2;
	m_pref_QueryTimeout=10;
	m_pref_QueryDisableTime=0;
	m_pref_QueryAccept=0;
	m_pref_IdleTimeout=0;
	m_pref_MaxViewerSetting = 0;
	m_pref_MaxViewers = 128;
	m_pref_Collabo = false;

	m_pref_Frame = FALSE;
	m_pref_Notification = FALSE;
	m_pref_OSD = FALSE;
	m_pref_NotificationSelection = 0;

	m_pref_EnableRemoteInputs=TRUE;
	m_pref_DisableLocalInputs=FALSE;
	m_pref_EnableJapInput=FALSE;
	m_pref_EnableUnicodeInput=FALSE;
	m_pref_EnableWin8Helper=FALSE;
	m_pref_clearconsole=FALSE;
	m_pref_LockSettings=-1;

	m_pref_RemoveWallpaper=FALSE;
	// adzm - 2010-07 - Disable more effects or font smoothing
	m_pref_RemoveEffects=FALSE;
	m_pref_RemoveFontSmoothing=FALSE;
    m_alloweditclients = TRUE;
	m_allowshutdown = TRUE;
	m_allowproperties = TRUE;
	m_allowInjection = FALSE;

	// Modif sf@2002
	// [v1.0.2-jp2 fix] Move to vncpropertiesPoll.cpp
//	m_pref_SingleWindow = FALSE;
	m_pref_UseDSMPlugin = FALSE;
	*m_pref_szDSMPlugin = '\0';
	m_pref_DSMPluginConfig[0] = '\0';

	m_pref_EnableFileTransfer = TRUE;
	m_pref_FTUserImpersonation = TRUE;
	m_pref_EnableBlankMonitor = TRUE;
	m_pref_BlankInputsOnly = FALSE;
	m_pref_QueryIfNoLogon = FALSE;
	m_pref_DefaultScale = 1;

	// Load the local prefs for this user
	if (hkDefault != NULL)
	{
		vnclog.Print(LL_INTINFO, VNCLOG("***** DBG - Local Preferences - Default\n"));

		vnclog.Print(LL_INTINFO, VNCLOG("loading DEFAULT local settings\n"));
		LoadUserPrefs(hkDefault);
		m_allowshutdown = LoadInt(hkDefault, "AllowShutdown", m_allowshutdown);
		m_allowproperties = LoadInt(hkDefault, "AllowProperties", m_allowproperties);
		m_allowInjection = LoadInt(hkDefault, "AllowInjection", m_allowInjection);
		m_alloweditclients = LoadInt(hkDefault, "AllowEditClients", m_alloweditclients);
	}

	// Are we being asked to load the user settings, or just the default local system settings?
	if (usersettings)
	{
		// We want the user settings, so load them!
		vnclog.Print(LL_INTINFO, VNCLOG("***** DBG - User Settings on\n"));

		if (hkLocalUser != NULL)
		{
			vnclog.Print(LL_INTINFO, VNCLOG("***** DBG - LoadUser Preferences\n"));

			vnclog.Print(LL_INTINFO, VNCLOG("loading \"%s\" local settings\n"), username);
			LoadUserPrefs(hkLocalUser);
			m_allowshutdown = LoadInt(hkLocalUser, "AllowShutdown", m_allowshutdown);
			m_allowproperties = LoadInt(hkLocalUser, "AllowProperties", m_allowproperties);
			m_allowInjection = LoadInt(hkLocalUser, "AllowInjection", m_allowInjection);
		  m_alloweditclients = LoadInt(hkLocalUser, "AllowEditClients", m_alloweditclients);
		}

		// Now override the system settings with the user's settings
		// If the username is SYSTEM then don't try to load them, because there aren't any...
		if (m_allowproperties && (strcmp(username, "SYSTEM") != 0))
		{
			vnclog.Print(LL_INTINFO, VNCLOG("***** DBG - Override system settings with users settings\n"));
			HKEY hkGlobalUser;
			if (RegCreateKeyEx(HKEY_CURRENT_USER,
				WINVNC_REGISTRY_KEY,
				0, REG_NONE, REG_OPTION_NON_VOLATILE,
				KEY_READ, NULL, &hkGlobalUser, &dw) == ERROR_SUCCESS)
			{
				vnclog.Print(LL_INTINFO, VNCLOG("loading \"%s\" global settings\n"), username);
				LoadUserPrefs(hkGlobalUser);
				RegCloseKey(hkGlobalUser);

				// Close the user registry hive so it can unload if required
				RegCloseKey(HKEY_CURRENT_USER);
			}
		}
	} else {
		vnclog.Print(LL_INTINFO, VNCLOG("***** DBG - User Settings off\n"));
		if (hkLocalUser != NULL)
		{
			vnclog.Print(LL_INTINFO, VNCLOG("loading \"%s\" local settings\n"), username);
			LoadUserPrefs(hkLocalUser);
			m_allowshutdown = LoadInt(hkLocalUser, "AllowShutdown", m_allowshutdown);
			m_allowproperties = LoadInt(hkLocalUser, "AllowProperties", m_allowproperties);
			m_allowInjection = LoadInt(hkLocalUser, "AllowInjection", m_allowInjection);
		    m_alloweditclients = LoadInt(hkLocalUser, "AllowEditClients", m_alloweditclients);			
		}
		vnclog.Print(LL_INTINFO, VNCLOG("bypassing user-specific settings (both local and global)\n"));
	}

	if (hkLocalUser != NULL) RegCloseKey(hkLocalUser);
	if (hkDefault != NULL) RegCloseKey(hkDefault);
	if (hkLocal != NULL) RegCloseKey(hkLocal);

	// Make the loaded settings active..
	ApplyUserPrefs();
}