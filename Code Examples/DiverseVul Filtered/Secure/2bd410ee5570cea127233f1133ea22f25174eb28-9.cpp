	bool AddListener(CWebSock& WebSock, CTemplate& Tmpl) {
		unsigned short uPort = WebSock.GetParam("port").ToUShort();
		CString sHost = WebSock.GetParam("host");
		if (sHost == "*") sHost = "";
		bool bSSL = WebSock.GetParam("ssl").ToBool();
		bool bIPv4 = WebSock.GetParam("ipv4").ToBool();
		bool bIPv6 = WebSock.GetParam("ipv6").ToBool();
		bool bIRC = WebSock.GetParam("irc").ToBool();
		bool bWeb = WebSock.GetParam("web").ToBool();

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
				WebSock.GetSession()->AddError("Choose either IPv4 or IPv6 or both.");
				return SettingsPage(WebSock, Tmpl);
			}
		}

		CListener::EAcceptType eAccept;
		if (bIRC) {
			if (bWeb) {
				eAccept = CListener::ACCEPT_ALL;
			} else {
				eAccept = CListener::ACCEPT_IRC;
			}
		} else {
			if (bWeb) {
				eAccept = CListener::ACCEPT_HTTP;
			} else {
				WebSock.GetSession()->AddError("Choose either IRC or Web or both.");
				return SettingsPage(WebSock, Tmpl);
			}
		}

		CString sMessage;
		if (CZNC::Get().AddListener(uPort, sHost, bSSL, eAddr, eAccept, sMessage)) {
			if (!sMessage.empty()) {
				WebSock.GetSession()->AddSuccess(sMessage);
			}
			if (!CZNC::Get().WriteConfig()) {
				WebSock.GetSession()->AddError("Port changed, but config was not written");
			}
		} else {
			WebSock.GetSession()->AddError(sMessage);
		}

		return SettingsPage(WebSock, Tmpl);
	}