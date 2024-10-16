	bool DelListener(CWebSock& WebSock, CTemplate& Tmpl) {
		unsigned short uPort = WebSock.GetParam("port").ToUShort();
		CString sHost = WebSock.GetParam("host");
		bool bIPv4 = WebSock.GetParam("ipv4").ToBool();
		bool bIPv6 = WebSock.GetParam("ipv6").ToBool();

		EAddrType eAddr = ADDR_ALL;
		if (bIPv4) {
			if (bIPv6) {
				eAddr = ADDR_ALL;
			} else {
				eAddr = ADDR_IPV4ONLY;
			}
		} else {
			if (bIPv6) {
				eAddr = ADDR_IPV6ONLY;
			} else {
				WebSock.GetSession()->AddError("Invalid request.");
				return SettingsPage(WebSock, Tmpl);
			}
		}

		CListener* pListener = CZNC::Get().FindListener(uPort, sHost, eAddr);
		if (pListener) {
			CZNC::Get().DelListener(pListener);
			if (!CZNC::Get().WriteConfig()) {
				WebSock.GetSession()->AddError("Port changed, but config was not written");
			}
		} else {
			WebSock.GetSession()->AddError("The specified listener was not found.");
		}

		return SettingsPage(WebSock, Tmpl);
	}