	bool DelNetwork(CWebSock& WebSock, CUser* pUser, CTemplate& Tmpl) {
		CString sNetwork = WebSock.GetParam("name");
		if (sNetwork.empty() && !WebSock.IsPost()) {
			sNetwork = WebSock.GetParam("name", false);
		}

		if (!pUser) {
			WebSock.PrintErrorPage("That user doesn't exist");
			return true;
		}

		if (sNetwork.empty()) {
			WebSock.PrintErrorPage("That network doesn't exist for this user");
			return true;
		}

		if (!WebSock.IsPost()) {
			// Show the "Are you sure?" page:

			Tmpl.SetFile("del_network.tmpl");
			Tmpl["Username"] = pUser->GetUserName();
			Tmpl["Network"] = sNetwork;
			return true;
		}

		pUser->DeleteNetwork(sNetwork);

		if (!CZNC::Get().WriteConfig()) {
			WebSock.PrintErrorPage("Network deleted, but config was not written");
			return true;
		}

		WebSock.Redirect("edituser?user=" + pUser->GetUserName().Escape_n(CString::EURL));
		return false;
	}