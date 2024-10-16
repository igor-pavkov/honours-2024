void vncProperties::LoadUserPrefsFromIniFile()
{
	// Modif sf@2002
	m_pref_EnableFileTransfer = myIniFile.ReadInt("admin", "FileTransferEnabled", m_pref_EnableFileTransfer);
	m_pref_FTUserImpersonation = myIniFile.ReadInt("admin", "FTUserImpersonation", m_pref_FTUserImpersonation); // sf@2005
	m_pref_EnableBlankMonitor = myIniFile.ReadInt("admin", "BlankMonitorEnabled", m_pref_EnableBlankMonitor);
	m_pref_BlankInputsOnly = myIniFile.ReadInt("admin", "BlankInputsOnly", m_pref_BlankInputsOnly); //PGM
	m_pref_DefaultScale = myIniFile.ReadInt("admin", "DefaultScale", m_pref_DefaultScale);

	m_pref_UseDSMPlugin = myIniFile.ReadInt("admin", "UseDSMPlugin", m_pref_UseDSMPlugin);
	myIniFile.ReadString("admin", "DSMPlugin",m_pref_szDSMPlugin,128);
	
	//adzm 2010-05-12 - dsmplugin config
	myIniFile.ReadString("admin", "DSMPluginConfig", m_pref_DSMPluginConfig, 512);
	
	m_pref_Primary = myIniFile.ReadInt("admin", "primary", m_pref_Primary);
	m_pref_Secondary = myIniFile.ReadInt("admin", "secondary", m_pref_Secondary);

	// Connection prefs
	m_pref_SockConnect=myIniFile.ReadInt("admin", "SocketConnect", m_pref_SockConnect);
	m_pref_HTTPConnect=myIniFile.ReadInt("admin", "HTTPConnect", m_pref_HTTPConnect);
	m_pref_AutoPortSelect=myIniFile.ReadInt("admin", "AutoPortSelect", m_pref_AutoPortSelect);
	m_pref_PortNumber=myIniFile.ReadInt("admin", "PortNumber", m_pref_PortNumber);
	m_pref_HttpPortNumber=myIniFile.ReadInt("admin", "HTTPPortNumber",
									DISPLAY_TO_HPORT(PORT_TO_DISPLAY(m_pref_PortNumber)));
	m_pref_IdleTimeout=myIniFile.ReadInt("admin", "IdleTimeout", m_pref_IdleTimeout);
	
	m_pref_RemoveWallpaper=myIniFile.ReadInt("admin", "RemoveWallpaper", m_pref_RemoveWallpaper);
	// adzm - 2010-07 - Disable more effects or font smoothing
	m_pref_RemoveEffects=myIniFile.ReadInt("admin", "RemoveEffects", m_pref_RemoveEffects);
	m_pref_RemoveFontSmoothing=myIniFile.ReadInt("admin", "RemoveFontSmoothing", m_pref_RemoveFontSmoothing);

	// Connection querying settings
	m_pref_QuerySetting=myIniFile.ReadInt("admin", "QuerySetting", m_pref_QuerySetting);
	m_server->SetQuerySetting(m_pref_QuerySetting);
	m_pref_QueryTimeout=myIniFile.ReadInt("admin", "QueryTimeout", m_pref_QueryTimeout);
	m_server->SetQueryTimeout(m_pref_QueryTimeout);
	m_pref_QueryDisableTime=myIniFile.ReadInt("admin", "QueryDisableTime", m_pref_QueryDisableTime);
	m_server->SetQueryDisableTime(m_pref_QueryDisableTime);	
	m_pref_QueryAccept=myIniFile.ReadInt("admin", "QueryAccept", m_pref_QueryAccept);
	m_server->SetQueryAccept(m_pref_QueryAccept);

	m_pref_MaxViewerSetting = myIniFile.ReadInt("admin", "MaxViewerSetting", m_pref_MaxViewerSetting);
	m_server->setMaxViewerSetting(m_pref_MaxViewerSetting);
	m_pref_MaxViewers = myIniFile.ReadInt("admin", "MaxViewers", m_pref_MaxViewers);
	m_server->setMaxViewers(m_pref_MaxViewers);

	m_pref_Collabo = myIniFile.ReadInt("admin", "Collabo", m_pref_Collabo);
	m_server->setCollabo(m_pref_Collabo);

	m_pref_Frame = myIniFile.ReadInt("admin", "Frame", m_pref_Frame);
	m_server->setFrame(m_pref_Frame);
	m_pref_Notification = myIniFile.ReadInt("admin", "Notification", m_pref_Notification);
	m_server->setNotification(m_pref_Notification);

	m_pref_OSD = myIniFile.ReadInt("admin", "OSD", m_pref_OSD);
	m_server->setOSD(m_pref_OSD);
	m_pref_NotificationSelection = myIniFile.ReadInt("admin", "NotificationSelection", m_pref_NotificationSelection);
	m_server->setNotificationSelection(m_pref_NotificationSelection);

	// marscha@2006 - Is AcceptDialog required even if no user is logged on
	m_pref_QueryIfNoLogon=myIniFile.ReadInt("admin", "QueryIfNoLogon", m_pref_QueryIfNoLogon);
	m_server->SetQueryIfNoLogon(m_pref_QueryIfNoLogon);

	// Load the password
	myIniFile.ReadPassword(m_pref_passwd,MAXPWLEN);
	myIniFile.ReadPassword2(m_pref_passwd2,MAXPWLEN); //PGM

	// Remote access prefs
	m_pref_EnableRemoteInputs=myIniFile.ReadInt("admin", "InputsEnabled", m_pref_EnableRemoteInputs);
	m_pref_LockSettings=myIniFile.ReadInt("admin", "LockSetting", m_pref_LockSettings);
	m_pref_DisableLocalInputs=myIniFile.ReadInt("admin", "LocalInputsDisabled", m_pref_DisableLocalInputs);
	m_pref_EnableJapInput=myIniFile.ReadInt("admin", "EnableJapInput", m_pref_EnableJapInput);
	m_pref_EnableUnicodeInput=myIniFile.ReadInt("admin", "EnableUnicodeInput", m_pref_EnableUnicodeInput);
	m_pref_EnableWin8Helper=myIniFile.ReadInt("admin", "EnableWin8Helper", m_pref_EnableWin8Helper);
	m_pref_clearconsole=myIniFile.ReadInt("admin", "clearconsole", m_pref_clearconsole);
	G_SENDBUFFER_EX=myIniFile.ReadInt("admin", "sendbuffer", G_SENDBUFFER_EX);
}