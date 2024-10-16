void vncProperties::SaveUserPrefsToIniFile()
{
	// SAVE THE PER USER PREFS
	vnclog.Print(LL_INTINFO, VNCLOG("saving current settings to registry\n"));

	// Modif sf@2002
	myIniFile.WriteInt("admin", "FileTransferEnabled", m_server->FileTransferEnabled());
	myIniFile.WriteInt("admin", "FTUserImpersonation", m_server->FTUserImpersonation()); // sf@2005
	myIniFile.WriteInt("admin", "BlankMonitorEnabled", m_server->BlankMonitorEnabled());
	myIniFile.WriteInt("admin", "BlankInputsOnly", m_server->BlankInputsOnly()); //PGM

	myIniFile.WriteInt("admin", "DefaultScale", m_server->GetDefaultScale());

	myIniFile.WriteInt("admin", "UseDSMPlugin", m_server->IsDSMPluginEnabled());

	myIniFile.WriteString("admin", "DSMPlugin",m_server->GetDSMPluginName());

	//adzm 2010-05-12 - dsmplugin config
	//myIniFile.WriteString("admin", "DSMPluginConfig", m_server->GetDSMPluginConfig());

	myIniFile.WriteInt("admin", "primary", m_server->Primary());
	myIniFile.WriteInt("admin", "secondary", m_server->Secondary());

	// Connection prefs
	myIniFile.WriteInt("admin", "SocketConnect", m_server->SockConnected());
	myIniFile.WriteInt("admin", "HTTPConnect", m_server->HTTPConnectEnabled());
	myIniFile.WriteInt("admin", "AutoPortSelect", m_server->AutoPortSelect());
	if (!m_server->AutoPortSelect()) {
		myIniFile.WriteInt("admin", "PortNumber", m_server->GetPort());
		myIniFile.WriteInt("admin", "HTTPPortNumber", m_server->GetHttpPort());
	}
	myIniFile.WriteInt("admin", "InputsEnabled", m_server->RemoteInputsEnabled());
	myIniFile.WriteInt("admin", "LocalInputsDisabled", m_server->LocalInputsDisabled());
	myIniFile.WriteInt("admin", "IdleTimeout", m_server->AutoIdleDisconnectTimeout());
	myIniFile.WriteInt("admin", "EnableJapInput", m_server->JapInputEnabled());
	myIniFile.WriteInt("admin", "EnableUnicodeInput", m_server->UnicodeInputEnabled());
	myIniFile.WriteInt("admin", "EnableWin8Helper", m_server->Win8HelperEnabled());

	// Connection querying settings
	myIniFile.WriteInt("admin", "QuerySetting", m_server->QuerySetting());
	myIniFile.WriteInt("admin", "QueryTimeout", m_server->QueryTimeout());
	myIniFile.WriteInt("admin", "QueryDisableTime", m_server->QueryDisableTime());
	myIniFile.WriteInt("admin", "QueryAccept", m_server->QueryAcceptForSave());
	myIniFile.WriteInt("admin", "MaxViewerSetting", m_server->getMaxViewerSetting());
	myIniFile.WriteInt("admin", "MaxViewers", m_server->getMaxViewers());
	myIniFile.WriteInt("admin", "Collabo", m_server->getCollabo());
	myIniFile.WriteInt("admin", "Frame", m_server->getFrame());
	myIniFile.WriteInt("admin", "Notification", m_server->getNotification());
	myIniFile.WriteInt("admin", "OSD", m_server->getOSD());
	myIniFile.WriteInt("admin", "NotificationSelection", m_server->getNotificationSelection());
	// Lock settings
	myIniFile.WriteInt("admin", "LockSetting", m_server->LockSettings());

	// Wallpaper removal
	myIniFile.WriteInt("admin", "RemoveWallpaper", m_server->RemoveWallpaperEnabled());
	// UI Effects
	// adzm - 2010-07 - Disable more effects or font smoothing
	myIniFile.WriteInt("admin", "RemoveEffects", m_server->RemoveEffectsEnabled());
	myIniFile.WriteInt("admin", "RemoveFontSmoothing", m_server->RemoveFontSmoothingEnabled());

	// Save the password
	char passwd[MAXPWLEN];
	m_server->GetPassword(passwd);
	myIniFile.WritePassword(passwd);
	memset(passwd, '\0', MAXPWLEN); //PGM
	m_server->GetPassword2(passwd); //PGM
	myIniFile.WritePassword2(passwd); //PGM
}