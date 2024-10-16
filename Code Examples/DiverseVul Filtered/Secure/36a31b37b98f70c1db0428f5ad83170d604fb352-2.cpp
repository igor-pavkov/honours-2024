vncProperties::SaveDSMPluginName(HKEY key, char *buffer)
{
	RegSetValueEx(key, "DSMPlugin", 0, REG_BINARY, (LPBYTE) buffer, MAXPATH);
}