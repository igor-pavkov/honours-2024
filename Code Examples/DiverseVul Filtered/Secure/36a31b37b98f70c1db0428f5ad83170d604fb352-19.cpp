vncProperties::LoadUserPrefs(HKEY appkey)
{
	// LOAD USER PREFS FROM THE SELECTED KEY

	// Modif sf@2002
	m_pref_EnableFileTransfer = LoadInt(appkey, "FileTransferEnabled", m_pref_EnableFileTransfer);
	m_pref_FTUserImpersonation = LoadInt(appkey, "FTUserImpersonation", m_pref_FTUserImpersonation); // sf@2005
	m_pref_EnableBlankMonitor = LoadInt(appkey, "BlankMonitorEnabled", m_pref_EnableBlankMonitor);
	m_pref_BlankInputsOnly = LoadInt(appkey, "BlankInputsOnly", m_pref_BlankInputsOnly); //PGM
	m_pref_DefaultScale = LoadInt(appkey, "DefaultScale", m_pref_DefaultScale);
	
	m_pref_Primary=LoadInt(appkey, "primary", m_pref_Primary);
	m_pref_Secondary=LoadInt(appkey, "secondary", m_pref_Secondary);

	m_pref_UseDSMPlugin = LoadInt(appkey, "UseDSMPlugin", m_pref_UseDSMPlugin);
	LoadDSMPluginName(appkey, m_pref_szDSMPlugin);

	// Connection prefs
	m_pref_SockConnect=LoadInt(appkey, "SocketConnect", m_pref_SockConnect);
	m_pref_HTTPConnect=LoadInt(appkey, "HTTPConnect", m_pref_HTTPConnect);
	m_pref_AutoPortSelect=LoadInt(appkey, "AutoPortSelect", m_pref_AutoPortSelect);
	m_pref_PortNumber=LoadInt(appkey, "PortNumber", m_pref_PortNumber);
	m_pref_HttpPortNumber=LoadInt(appkey, "HTTPPortNumber",
									DISPLAY_TO_HPORT(PORT_TO_DISPLAY(m_pref_PortNumber)));
	m_pref_IdleTimeout=LoadInt(appkey, "IdleTimeout", m_pref_IdleTimeout);
	
	m_pref_RemoveWallpaper=LoadInt(appkey, "RemoveWallpaper", m_pref_RemoveWallpaper);
	// adzm - 2010-07 - Disable more effects or font smoothing
	m_pref_RemoveEffects=LoadInt(appkey, "RemoveEffects", m_pref_RemoveEffects);
	m_pref_RemoveFontSmoothing=LoadInt(appkey, "RemoveFontSmoothing", m_pref_RemoveFontSmoothing);

	// Connection querying settings
	m_pref_QuerySetting=LoadInt(appkey, "QuerySetting", m_pref_QuerySetting);
	m_server->SetQuerySetting(m_pref_QuerySetting);
	m_pref_QueryTimeout=LoadInt(appkey, "QueryTimeout", m_pref_QueryTimeout);
	m_server->SetQueryTimeout(m_pref_QueryTimeout);
	m_pref_QueryDisableTime=LoadInt(appkey, "QueryDisableTime", m_pref_QueryDisableTime);
	m_server->SetQueryDisableTime(m_pref_QueryDisableTime);
	m_pref_QueryAccept=LoadInt(appkey, "QueryAccept", m_pref_QueryAccept);
	m_server->SetQueryAccept(m_pref_QueryAccept);

	m_pref_MaxViewerSetting = LoadInt(appkey, "MaxViewerSetting", m_pref_MaxViewerSetting);
	m_server->setMaxViewerSetting(m_pref_MaxViewerSetting);
	m_pref_MaxViewers = LoadInt(appkey, "MaxViewers", m_pref_MaxViewers);
	m_server->setMaxViewers(m_pref_MaxViewers);	
	m_pref_Collabo = LoadInt(appkey, "Collabo", m_pref_Collabo);
	m_server->setCollabo(m_pref_Collabo);

	m_pref_Frame = LoadInt(appkey, "Frame", m_pref_Frame);
	m_server->setFrame(m_pref_Frame);
	m_pref_Notification = LoadInt(appkey, "Notification", m_pref_Notification);
	m_server->setNotification(m_pref_Notification);
	m_pref_OSD = LoadInt(appkey, "OSD", m_pref_OSD);
	m_server->setOSD(m_pref_OSD);
	m_pref_NotificationSelection = LoadInt(appkey, "NotificationSelection", m_pref_NotificationSelection);
	m_server->setNotificationSelection(m_pref_NotificationSelection);
	

	// marscha@2006 - Is AcceptDialog required even if no user is logged on
	m_pref_QueryIfNoLogon=LoadInt(appkey, "QueryIfNoLogon", m_pref_QueryIfNoLogon);
	m_server->SetQueryIfNoLogon(m_pref_QueryIfNoLogon);

	// Load the password
	LoadPassword(appkey, m_pref_passwd);
	LoadPassword2(appkey, m_pref_passwd2); //PGM

	// Remote access prefs
	m_pref_EnableRemoteInputs=LoadInt(appkey, "InputsEnabled", m_pref_EnableRemoteInputs);
	m_pref_LockSettings=LoadInt(appkey, "LockSetting", m_pref_LockSettings);
	m_pref_DisableLocalInputs=LoadInt(appkey, "LocalInputsDisabled", m_pref_DisableLocalInputs);
	m_pref_EnableJapInput=LoadInt(appkey, "EnableJapInput", m_pref_EnableJapInput);
	m_pref_EnableUnicodeInput=LoadInt(appkey, "EnableUnicodeInput", m_pref_EnableUnicodeInput);
	m_pref_EnableWin8Helper=LoadInt(appkey, "EnableWin8Helper", m_pref_EnableWin8Helper);
	m_pref_clearconsole=LoadInt(appkey, "clearconsole", m_pref_clearconsole);
}