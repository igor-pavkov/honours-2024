vncProperties::LoadDSMPluginName(HKEY key, char *buffer)
{
	DWORD type = REG_BINARY;
	int slen=MAXPATH;
	char inouttext[MAXPATH];

	if (RegQueryValueEx(key,
		"DSMPlugin",
		NULL,
		&type,
		(LPBYTE) &inouttext,
		(LPDWORD) &slen) != ERROR_SUCCESS)
		return;

	if (slen > MAXPATH)
		return;

	memcpy(buffer, inouttext, MAXPATH);
}