bool CWebSock::AddModLoop(const CString& sLoopName, CModule& Module,
                          CTemplate* pTemplate) {
    if (!pTemplate) {
        pTemplate = &m_Template;
    }

    CString sTitle(Module.GetWebMenuTitle());

    if (!sTitle.empty() && (IsLoggedIn() || (!Module.WebRequiresLogin() &&
                                             !Module.WebRequiresAdmin())) &&
        (GetSession()->IsAdmin() || !Module.WebRequiresAdmin())) {
        CTemplate& Row = pTemplate->AddRow(sLoopName);
        bool bActiveModule = false;

        Row["ModName"] = Module.GetModName();
        Row["ModPath"] = Module.GetWebPath();
        Row["Title"] = sTitle;

        if (m_sModName == Module.GetModName()) {
            CString sModuleType = GetPath().Token(1, false, "/");
            if (sModuleType == "global" &&
                Module.GetType() == CModInfo::GlobalModule) {
                bActiveModule = true;
            } else if (sModuleType == "user" &&
                       Module.GetType() == CModInfo::UserModule) {
                bActiveModule = true;
            } else if (sModuleType == "network" &&
                       Module.GetType() == CModInfo::NetworkModule) {
                CIRCNetwork* Network = Module.GetNetwork();
                if (Network) {
                    CString sNetworkName = GetPath().Token(2, false, "/");
                    if (sNetworkName == Network->GetName()) {
                        bActiveModule = true;
                    }
                } else {
                    bActiveModule = true;
                }
            }
        }

        if (bActiveModule) {
            Row["Active"] = "true";
        }

        if (Module.GetUser()) {
            Row["Username"] = Module.GetUser()->GetUserName();
        }

        VWebSubPages& vSubPages = Module.GetSubPages();

        for (TWebSubPage& SubPage : vSubPages) {
            // bActive is whether or not the current url matches this subpage
            // (params will be checked below)
            bool bActive = (m_sModName == Module.GetModName() &&
                            m_sPage == SubPage->GetName() && bActiveModule);

            if (SubPage->RequiresAdmin() && !GetSession()->IsAdmin()) {
                // Don't add admin-only subpages to requests from non-admin
                // users
                continue;
            }

            CTemplate& SubRow = Row.AddRow("SubPageLoop");

            SubRow["ModName"] = Module.GetModName();
            SubRow["ModPath"] = Module.GetWebPath();
            SubRow["PageName"] = SubPage->GetName();
            SubRow["Title"] = SubPage->GetTitle().empty() ? SubPage->GetName()
                                                          : SubPage->GetTitle();

            CString& sParams = SubRow["Params"];

            const VPair& vParams = SubPage->GetParams();
            for (const pair<CString, CString>& ssNV : vParams) {
                if (!sParams.empty()) {
                    sParams += "&";
                }

                if (!ssNV.first.empty()) {
                    if (!ssNV.second.empty()) {
                        sParams += ssNV.first.Escape_n(CString::EURL);
                        sParams += "=";
                        sParams += ssNV.second.Escape_n(CString::EURL);
                    }

                    if (bActive && GetParam(ssNV.first, false) != ssNV.second) {
                        bActive = false;
                    }
                }
            }

            if (bActive) {
                SubRow["Active"] = "true";
            }
        }

        return true;
    }

    return false;
}