vncProperties::SaveString(HKEY key,LPCSTR valname, const char *buffer)
{
	RegSetValueEx(key, valname, 0, REG_BINARY, (LPBYTE) buffer, (DWORD)(strlen(buffer)+1));
}