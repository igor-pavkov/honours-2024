	bool ListUsersPage(CWebSock& WebSock, CTemplate& Tmpl) {
		CSmartPtr<CWebSession> spSession = WebSock.GetSession();
		const map<CString,CUser*>& msUsers = CZNC::Get().GetUserMap();
		Tmpl["Title"] = "List Users";
		Tmpl["Action"] = "listusers";

		unsigned int a = 0;

		for (map<CString,CUser*>::const_iterator it = msUsers.begin(); it != msUsers.end(); ++it, a++) {
			CTemplate& l = Tmpl.AddRow("UserLoop");
			CUser& User = *it->second;

			l["Username"] = User.GetUserName();
			l["Clients"] = CString(User.GetAllClients().size());
			l["Networks"] = CString(User.GetNetworks().size());

			if (&User == spSession->GetUser()) {
				l["IsSelf"] = "true";
			}
		}

		return true;
	}