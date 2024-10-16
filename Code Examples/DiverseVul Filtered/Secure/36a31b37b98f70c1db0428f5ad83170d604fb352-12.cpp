vncProperties::SaveUserPrefs(HKEY appkey)
{
	// SAVE THE PER USER PREFS
	vnclog.Print(LL_INTINFO, VNCLOG("saving current settings to registry\n"));

	// Modif sf@2002
	SaveInt(appkey, "FileTransferEnabled", m_server->FileTransferEnabled());
	SaveInt(appkey, "FTUserImpersonation", m_server->FTUserImpersonation()); // sf@2005
	SaveInt(appkey, "BlankMonitorEnabled", m_server->BlankMonitorEnabled());
	SaveInt(appkey, "BlankInputsOnly", m_server->BlankInputsOnly()); //PGM
	SaveInt(appkey, "primary", m_server->Primary());
	SaveInt(appkey, "secondary", m_server->Secondary());

	SaveInt(appkey, "DefaultScale", m_server->GetDefaultScale());

	SaveInt(appkey, "UseDSMPlugin", m_server->IsDSMPluginEnabled());
	SaveDSMPluginName(appkey, m_server->GetDSMPluginName());
	//adzm 2010-05-12 - dsmplugin config
	SaveString(appkey, "DSMPluginConfig", m_server->GetDSMPluginConfig());

	// Connection prefs
	SaveInt(appkey, "SocketConnect", m_server->SockConnected());
	SaveInt(appkey, "HTTPConnect", m_server->HTTPConnectEnabled());
	SaveInt(appkey, "AutoPortSelect", m_server->AutoPortSelect());
	if (!m_server->AutoPortSelect()) {
		SaveInt(appkey, "PortNumber", m_server->GetPort());
		SaveInt(appkey, "HTTPPortNumber", m_server->GetHttpPort());
	}
	SaveInt(appkey, "InputsEnabled", m_server->RemoteInputsEnabled());
	SaveInt(appkey, "LocalInputsDisabled", m_server->LocalInputsDisabled());
	SaveInt(appkey, "IdleTimeout", m_server->AutoIdleDisconnectTimeout());
	SaveInt(appkey, "EnableJapInput", m_server->JapInputEnabled());
	SaveInt(appkey, "EnableUnicodeInput", m_server->UnicodeInputEnabled());
	SaveInt(appkey, "EnableWin8Helper", m_server->Win8HelperEnabled());

	// Connection querying settings
	SaveInt(appkey, "QuerySetting", m_server->QuerySetting());
	SaveInt(appkey, "QueryTimeout", m_server->QueryTimeout());
	SaveInt(appkey, "QueryDisableTime", m_server->QueryDisableTime());
	SaveInt(appkey, "QueryAccept", m_server->QueryAcceptForSave());
	SaveInt(appkey, "MaxViewerSetting", m_server->getMaxViewerSetting());
	SaveInt(appkey, "MaxViewers", m_server->getMaxViewers());
	SaveInt(appkey, "Collabo", m_server->getCollabo());
	SaveInt(appkey, "Frame", m_server->getFrame());
	SaveInt(appkey, "Notification", m_server->getNotification());
	SaveInt(appkey, "OSD", m_server->getOSD());
	SaveInt(appkey, "NotificationSelection", m_server->getNotificationSelection());


	// Lock settings
	SaveInt(appkey, "LockSetting", m_server->LockSettings());

	// Wallpaper removal
	SaveInt(appkey, "RemoveWallpaper", m_server->RemoveWallpaperEnabled());
	// UI Effects
	// adzm - 2010-07 - Disable more effects or font smoothing
	SaveInt(appkey, "RemoveEffects", m_server->RemoveEffectsEnabled());
	SaveInt(appkey, "RemoveFontSmoothing", m_server->RemoveFontSmoothingEnabled());

	// Save the password
	char passwd[MAXPWLEN];
	m_server->GetPassword(passwd);
	SavePassword(appkey, passwd);
	memset(passwd, '\0', MAXPWLEN); //PGM
	m_server->GetPassword2(passwd); //PGM
	SavePassword2(appkey, passwd); //PGM
}