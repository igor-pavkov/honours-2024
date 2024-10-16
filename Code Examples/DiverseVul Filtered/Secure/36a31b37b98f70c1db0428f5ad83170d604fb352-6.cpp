vncProperties::SavePassword2(HKEY key, char *buffer) //PGM
{ //PGM
	RegSetValueEx(key, "Password2", 0, REG_BINARY, (LPBYTE) buffer, MAXPWLEN); //PGM
} //PGM