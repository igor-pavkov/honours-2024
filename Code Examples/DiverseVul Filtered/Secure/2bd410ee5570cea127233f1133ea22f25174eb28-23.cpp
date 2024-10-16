	CIRCNetwork* SafeGetNetworkFromParam(CWebSock& WebSock) {
		CUser* pUser = CZNC::Get().FindUser(SafeGetUserNameParam(WebSock));
		CIRCNetwork* pNetwork = NULL;

		if (pUser) {
			pNetwork = pUser->FindNetwork(SafeGetNetworkParam(WebSock));
		}

		return pNetwork;
	}