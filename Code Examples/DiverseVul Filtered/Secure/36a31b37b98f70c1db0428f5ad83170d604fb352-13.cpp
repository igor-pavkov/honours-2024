vncProperties::vncProperties()
{
    m_alloweditclients = TRUE;
	m_allowproperties = TRUE;
	m_allowInjection = FALSE;
	m_allowshutdown = TRUE;
	m_dlgvisible = FALSE;
	m_usersettings = TRUE;
	Lock_service_helper=TRUE;
	m_fUseRegistry = FALSE;
    m_ftTimeout = FT_RECV_TIMEOUT;
    m_keepAliveInterval = KEEPALIVE_INTERVAL;
	m_IdleInputTimeout = 0;
	m_pref_Primary=true;
	m_pref_Secondary=false;

	m_pref_DSMPluginConfig[0] = '\0';
	hBmpExpand = (HBITMAP)::LoadImage(hInstResDLL, MAKEINTRESOURCE(IDB_EXPAND), IMAGE_BITMAP, 0, 0, LR_LOADTRANSPARENT);
	hBmpCollaps = (HBITMAP)::LoadImage(hInstResDLL, MAKEINTRESOURCE(IDB_COLLAPS), IMAGE_BITMAP, 0, 0, LR_LOADTRANSPARENT);
	m_bExpanded = true;	
	cy = 0;
	cx = 0;
	service_commandline[0] = '\0';
	accept_reject_mesg[0] = '\0';

}