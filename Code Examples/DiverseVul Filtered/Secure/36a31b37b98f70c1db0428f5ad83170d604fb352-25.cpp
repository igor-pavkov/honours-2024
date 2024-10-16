void vncProperties::LoadFromIniFile()
{
	//if (m_dlgvisible)
	//{
	//	vnclog.Print(LL_INTWARN, VNCLOG("service helper invoked while Properties panel displayed\n"));
	//	return;
	//}

	char username[UNLEN+1];

	// Get the user name / service name
	if (!vncService::CurrentUser((char *)&username, sizeof(username)))
	{
		vnclog.Print(LL_INTINFO, VNCLOG("***** DBG - NO current user\n"));
		return;
	}

	// If there is no user logged on them default to SYSTEM
	if (strcmp(username, "") == 0)
	{
		//vnclog.Print(LL_INTINFO, VNCLOG("***** DBG - Force USER SYSTEM 2\n"));
		strcpy_s((char *)&username, UNLEN+1, "SYSTEM");
	}

	// Logging/debugging prefs
	vnclog.SetMode(myIniFile.ReadInt("admin", "DebugMode", 0));
	char temp[512];
	myIniFile.ReadString("admin", "path", temp,512);	
	vnclog.SetPath(temp);
	vnclog.SetLevel(myIniFile.ReadInt("admin", "DebugLevel", 0));
	vnclog.SetVideo(myIniFile.ReadInt("admin", "Avilog", 0) ? true : false);

	// Disable Tray Icon
	m_server->SetDisableTrayIcon(myIniFile.ReadInt("admin", "DisableTrayIcon", false));
	m_server->SetRdpmode(myIniFile.ReadInt("admin", "rdpmode", 0));
	m_server->SetNoScreensaver(myIniFile.ReadInt("admin", "noscreensaver", 0));

	// Authentication required, loopback allowed, loopbackOnly

	m_server->SetLoopbackOnly(myIniFile.ReadInt("admin", "LoopbackOnly", false));

	m_pref_Secure = false;
	m_pref_Secure = myIniFile.ReadInt("admin", "Secure", m_pref_Secure);
	m_server->Secure(m_pref_Secure);

	m_pref_RequireMSLogon=false;
	m_pref_RequireMSLogon = myIniFile.ReadInt("admin", "MSLogonRequired", m_pref_RequireMSLogon);
	m_server->RequireMSLogon(m_pref_RequireMSLogon);

	// Marscha@2004 - authSSP: added NewMSLogon checkbox to admin props page
	m_pref_NewMSLogon = false;
	m_pref_NewMSLogon = myIniFile.ReadInt("admin", "NewMSLogon", m_pref_NewMSLogon);
	m_server->SetNewMSLogon(m_pref_NewMSLogon);

	// sf@2003 - Moved DSM params here
	m_pref_UseDSMPlugin=false;
	m_pref_UseDSMPlugin = myIniFile.ReadInt("admin", "UseDSMPlugin", m_pref_UseDSMPlugin);
	myIniFile.ReadString("admin", "DSMPlugin",m_pref_szDSMPlugin,128);

	m_pref_ReverseAuthRequired = true;
	m_pref_ReverseAuthRequired = myIniFile.ReadInt("admin", "ReverseAuthRequired", m_pref_ReverseAuthRequired);
	m_server->SetReverseAuthRequired(m_pref_ReverseAuthRequired);
	
	//adzm 2010-05-12 - dsmplugin config
	myIniFile.ReadString("admin", "DSMPluginConfig", m_pref_DSMPluginConfig, 512);
#ifdef IPV6V4
	m_server->SetIPV6(myIniFile.ReadInt("admin", "UseIpv6", false));
#endif
	if (m_server->LoopbackOnly()) m_server->SetLoopbackOk(true);
	else m_server->SetLoopbackOk(myIniFile.ReadInt("admin", "AllowLoopback", true));
	m_server->SetAuthRequired(myIniFile.ReadInt("admin", "AuthRequired", true));

	m_server->SetConnectPriority(myIniFile.ReadInt("admin", "ConnectPriority", 0));
	if (!m_server->LoopbackOnly())
	{
		char *authhosts=new char[150];
		myIniFile.ReadString("admin", "AuthHosts",authhosts,150);
		if (authhosts != 0) {
			m_server->SetAuthHosts(authhosts);
			delete [] authhosts;
		} else {
			m_server->SetAuthHosts(0);
		}
	} else {
		m_server->SetAuthHosts(0);
	}

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
	m_pref_EnableRemoteInputs=TRUE;
	m_pref_DisableLocalInputs=FALSE;
	m_pref_EnableJapInput=FALSE;
	m_pref_EnableUnicodeInput=FALSE;
	m_pref_EnableWin8Helper=FALSE;
	m_pref_clearconsole=FALSE;
	m_pref_LockSettings=-1;
	m_pref_Collabo=false;

	m_pref_Frame = FALSE;
	m_pref_Notification = FALSE;
	m_pref_OSD = FALSE;
	m_pref_NotificationSelection = 0;

	m_pref_RemoveWallpaper=FALSE;
	// adzm - 2010-07 - Disable more effects or font smoothing
	m_pref_RemoveEffects=FALSE;
	m_pref_RemoveFontSmoothing=FALSE;
    m_alloweditclients = TRUE;
	m_allowshutdown = TRUE;
	m_allowproperties = TRUE;
	m_allowInjection = FALSE;

	// Modif sf@2002
	m_pref_SingleWindow = FALSE;
	m_pref_UseDSMPlugin = FALSE;
	*m_pref_szDSMPlugin = '\0';
	m_pref_DSMPluginConfig[0] = '\0';

	m_pref_EnableFileTransfer = TRUE;
	m_pref_FTUserImpersonation = TRUE;
	m_pref_EnableBlankMonitor = TRUE;
	m_pref_BlankInputsOnly = FALSE;
	m_pref_QueryIfNoLogon = FALSE;
	m_pref_DefaultScale = 1;

	LoadUserPrefsFromIniFile();
	m_allowshutdown = myIniFile.ReadInt("admin", "AllowShutdown", m_allowshutdown);
	m_allowproperties = myIniFile.ReadInt("admin", "AllowProperties", m_allowproperties);
	m_allowInjection = myIniFile.ReadInt("admin", "AllowInjection", m_allowInjection);
	m_alloweditclients = myIniFile.ReadInt("admin", "AllowEditClients", m_alloweditclients);

    m_ftTimeout = myIniFile.ReadInt("admin", "FileTransferTimeout", m_ftTimeout);
    if (m_ftTimeout > 600)
        m_ftTimeout = 600;

    m_keepAliveInterval = myIniFile.ReadInt("admin", "KeepAliveInterval", m_keepAliveInterval);
	m_IdleInputTimeout = myIniFile.ReadInt("admin", "IdleInputTimeout", m_IdleInputTimeout);

    if (m_keepAliveInterval >= (m_ftTimeout - KEEPALIVE_HEADROOM))
        m_keepAliveInterval = m_ftTimeout - KEEPALIVE_HEADROOM;

    m_server->SetFTTimeout(m_ftTimeout);
    m_server->SetKeepAliveInterval(m_keepAliveInterval);
	m_server->SetIdleInputTimeout(m_IdleInputTimeout);
    
	myIniFile.ReadString("admin", "service_commandline", service_commandline, 1024);
	myIniFile.ReadString("admin", "accept_reject_mesg", accept_reject_mesg, 512);

	ApplyUserPrefs();
}