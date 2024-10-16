void vncProperties::ReloadDynamicSettings()
{
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
	vnclog.SetLevel(myIniFile.ReadInt("admin", "DebugLevel", 0));
}