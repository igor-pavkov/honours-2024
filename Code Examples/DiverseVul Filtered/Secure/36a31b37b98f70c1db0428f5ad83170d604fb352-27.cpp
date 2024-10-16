void vncProperties::SaveToIniFile()
{
	if (!m_allowproperties)
		return;

	// SAVE PER-USER PREFS IF ALLOWED
	if (!myIniFile.IsWritable()  || vncService::RunningAsService())
			{
				//First check if temp file is writable
				myIniFile.IniFileSetTemp( m_Tempfile);
				if (!myIniFile.IsWritable())
					{
						vnclog.Print(LL_INTERR, VNCLOG("file %s not writable, error saving new settings\n"), m_Tempfile);
						return;				
					}
				if (!Copy_to_Temp( m_Tempfile))
					{
						vnclog.Print(LL_INTERR, VNCLOG("file %s not writable, error saving new settings\n"), m_Tempfile);
						return;				
					}

				SaveUserPrefsToIniFile();
				myIniFile.WriteInt("admin", "DebugMode", vnclog.GetMode());
				myIniFile.WriteInt("admin", "Avilog", vnclog.GetVideo());
				myIniFile.WriteString("admin", "path", vnclog.GetPath());
				myIniFile.WriteInt("admin", "DebugLevel", vnclog.GetLevel());
				myIniFile.WriteInt("admin", "AllowLoopback", m_server->LoopbackOk());
#ifdef IPV6V4
				myIniFile.WriteInt("admin", "UseIpv6", m_server->IPV6());
#endif
				myIniFile.WriteInt("admin", "LoopbackOnly", m_server->LoopbackOnly());
				myIniFile.WriteInt("admin", "AllowShutdown", m_allowshutdown);
				myIniFile.WriteInt("admin", "AllowProperties",  m_allowproperties);
				myIniFile.WriteInt("admin", "AllowInjection",  m_allowInjection);				
				myIniFile.WriteInt("admin", "AllowEditClients", m_alloweditclients);
				myIniFile.WriteInt("admin", "FileTransferTimeout", m_ftTimeout);
				myIniFile.WriteInt("admin", "KeepAliveInterval", m_keepAliveInterval);
				myIniFile.WriteInt("admin", "IdleInputTimeout", m_IdleInputTimeout);
				myIniFile.WriteInt("admin", "DisableTrayIcon", m_server->GetDisableTrayIcon());
				myIniFile.WriteInt("admin", "rdpmode", m_server->GetRdpmode());
				myIniFile.WriteInt("admin", "noscreensaver", m_server->GetNoScreensaver());
				myIniFile.WriteInt("admin", "Secure", m_server->Secure());
				myIniFile.WriteInt("admin", "MSLogonRequired", m_server->MSLogonRequired());
				// Marscha@2004 - authSSP: save "New MS-Logon" state
				myIniFile.WriteInt("admin", "NewMSLogon", m_server->GetNewMSLogon());
				myIniFile.WriteInt("admin", "ReverseAuthRequired", m_server->GetReverseAuthRequired());
				// sf@2003 - DSM params here
				myIniFile.WriteInt("admin", "ConnectPriority", m_server->ConnectPriority());
				myIniFile.WriteString("admin", "service_commandline", service_commandline);
				myIniFile.WriteString("admin", "accept_reject_mesg", accept_reject_mesg);
				myIniFile.copy_to_secure();
				myIniFile.IniFileSetSecure();
				return;
			}

	SaveUserPrefsToIniFile();
	myIniFile.WriteInt("admin", "DebugMode", vnclog.GetMode());
	myIniFile.WriteInt("admin", "Avilog", vnclog.GetVideo());
	myIniFile.WriteString("admin", "path", vnclog.GetPath());
	myIniFile.WriteInt("admin", "DebugLevel", vnclog.GetLevel());
	myIniFile.WriteInt("admin", "AllowLoopback", m_server->LoopbackOk());
#ifdef IPV6V4
	myIniFile.WriteInt("admin", "UseIpv6", m_server->IPV6());
#endif
	myIniFile.WriteInt("admin", "LoopbackOnly", m_server->LoopbackOnly());
	myIniFile.WriteInt("admin", "AllowShutdown", m_allowshutdown);
	myIniFile.WriteInt("admin", "AllowProperties",  m_allowproperties);
	myIniFile.WriteInt("admin", "AllowInjection",  m_allowInjection);
	myIniFile.WriteInt("admin", "AllowEditClients", m_alloweditclients);
    myIniFile.WriteInt("admin", "FileTransferTimeout", m_ftTimeout);
    myIniFile.WriteInt("admin", "KeepAliveInterval", m_keepAliveInterval);
	myIniFile.WriteInt("admin", "IdleInputTimeout", m_IdleInputTimeout);
	myIniFile.WriteInt("admin", "DisableTrayIcon", m_server->GetDisableTrayIcon());
	myIniFile.WriteInt("admin", "rdpmode", m_server->GetRdpmode());
	myIniFile.WriteInt("admin", "noscreensaver", m_server->GetNoScreensaver());
	myIniFile.WriteInt("admin", "Secure", m_server->Secure());
	myIniFile.WriteInt("admin", "MSLogonRequired", m_server->MSLogonRequired());
	// Marscha@2004 - authSSP: save "New MS-Logon" state
	myIniFile.WriteInt("admin", "NewMSLogon", m_server->GetNewMSLogon());
	myIniFile.WriteInt("admin", "ReverseAuthRequired", m_server->GetReverseAuthRequired());
	// sf@2003 - DSM params here
	myIniFile.WriteInt("admin", "ConnectPriority", m_server->ConnectPriority());

	myIniFile.WriteString("admin", "service_commandline", service_commandline);
	myIniFile.WriteString("admin", "accept_reject_mesg", accept_reject_mesg);
	return;
}