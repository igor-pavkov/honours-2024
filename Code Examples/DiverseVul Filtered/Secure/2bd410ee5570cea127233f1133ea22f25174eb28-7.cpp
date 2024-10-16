	bool DelChan(CWebSock& WebSock, CIRCNetwork* pNetwork) {
		CString sChan = WebSock.GetParam("name", false);

		if (sChan.empty()) {
			WebSock.PrintErrorPage("That channel doesn't exist for this user");
			return true;
		}

		pNetwork->DelChan(sChan);
		pNetwork->PutIRC("PART " + sChan);

		if (!CZNC::Get().WriteConfig()) {
			WebSock.PrintErrorPage("Channel deleted, but config was not written");
			return true;
		}

		WebSock.Redirect("editnetwork?user=" + pNetwork->GetUser()->GetUserName().Escape_n(CString::EURL) + "&network=" + pNetwork->GetName().Escape_n(CString::EURL));
		return false;
	}