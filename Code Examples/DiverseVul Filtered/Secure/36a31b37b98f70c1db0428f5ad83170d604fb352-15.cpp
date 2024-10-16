vncProperties::LoadPassword2(HKEY key, char *buffer) //PGM
{ //PGM
	DWORD type = REG_BINARY; //PGM
	int slen=MAXPWLEN; //PGM
	char inouttext[MAXPWLEN]; //PGM

	// Retrieve the encrypted password //PGM
	if (RegQueryValueEx(key, //PGM
		"Password2", //PGM
		NULL, //PGM
		&type, //PGM
		(LPBYTE) &inouttext, //PGM
		(LPDWORD) &slen) != ERROR_SUCCESS) //PGM
		return; //PGM

	if (slen > MAXPWLEN) //PGM
		return; //PGM

	memcpy(buffer, inouttext, MAXPWLEN); //PGM
} //PGM