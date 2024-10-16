	CString SafeGetNetworkParam(CWebSock& WebSock) {
		CString sNetwork = WebSock.GetParam("network"); // check for POST param
		if(sNetwork.empty() && !WebSock.IsPost()) {
			// if no POST param named user has been given and we are not
			// saving this form, fall back to using the GET parameter.
			sNetwork = WebSock.GetParam("network", false);
		}
		return sNetwork;
	}