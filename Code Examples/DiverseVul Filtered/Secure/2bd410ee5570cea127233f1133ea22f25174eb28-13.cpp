	CString SafeGetUserNameParam(CWebSock& WebSock) {
		CString sUserName = WebSock.GetParam("user"); // check for POST param
		if(sUserName.empty() && !WebSock.IsPost()) {
			// if no POST param named user has been given and we are not
			// saving this form, fall back to using the GET parameter.
			sUserName = WebSock.GetParam("user", false);
		}
		return sUserName;
	}