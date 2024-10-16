	CUser* SafeGetUserFromParam(CWebSock& WebSock) {
		return CZNC::Get().FindUser(SafeGetUserNameParam(WebSock));
	}