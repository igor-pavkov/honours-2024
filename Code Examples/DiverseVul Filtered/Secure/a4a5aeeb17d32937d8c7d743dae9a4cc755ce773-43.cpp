CWebSock::EPageReqResult CWebSock::OnPageRequestInternal(const CString& sURI,
                                                         CString& sPageRet) {
    // Check that their session really belongs to their IP address. IP-based
    // authentication is bad, but here it's just an extra layer that makes
    // stealing cookies harder to pull off.
    //
    // When their IP is wrong, we give them an invalid cookie. This makes
    // sure that they will get a new cookie on their next request.
    if (CZNC::Get().GetProtectWebSessions() &&
        GetSession()->GetIP() != GetRemoteIP()) {
        DEBUG("Expected IP: " << GetSession()->GetIP());
        DEBUG("Remote IP:   " << GetRemoteIP());
        SendCookie("SessionId", "WRONG_IP_FOR_SESSION");
        PrintErrorPage(403, "Access denied",
                       "This session does not belong to your IP.");
        return PAGE_DONE;
    }

    // For pages *not provided* by modules, a CSRF check is performed which involves:
    // Ensure that they really POSTed from one our forms by checking if they
    // know the "secret" CSRF check value. Don't do this for login since
    // CSRF against the login form makes no sense and the login form does a
    // cookies-enabled check which would break otherwise.
    // Don't do this, if user authenticated using http-basic auth, because:
    // 1. they obviously know the password,
    // 2. it's easier to automate some tasks e.g. user creation, without need to
    //    care about cookies and CSRF
    if (IsPost() && !m_bBasicAuth && !sURI.StartsWith("/mods/") &&
        !ValidateCSRFCheck(sURI)) {
        DEBUG("Expected _CSRF_Check: " << GetCSRFCheck());
        DEBUG("Actual _CSRF_Check:   " << GetParam("_CSRF_Check"));
        PrintErrorPage(
            403, "Access denied",
            "POST requests need to send "
            "a secret token to prevent cross-site request forgery attacks.");
        return PAGE_DONE;
    }

    SendCookie("SessionId", GetSession()->GetId());

    if (GetSession()->IsLoggedIn()) {
        m_sUser = GetSession()->GetUser()->GetUserName();
        m_bLoggedIn = true;
    }
    CLanguageScope user_language(
        m_bLoggedIn ? GetSession()->GetUser()->GetLanguage() : "");

    // Handle the static pages that don't require a login
    if (sURI == "/") {
        if (!m_bLoggedIn && GetParam("cookie_check", false).ToBool() &&
            GetRequestCookie("SessionId").empty()) {
            GetSession()->AddError(
                "Your browser does not have cookies enabled for this site!");
        }
        return PrintTemplate("index", sPageRet);
    } else if (sURI == "/favicon.ico") {
        return PrintStaticFile("/pub/favicon.ico", sPageRet);
    } else if (sURI == "/robots.txt") {
        return PrintStaticFile("/pub/robots.txt", sPageRet);
    } else if (sURI == "/logout") {
        GetSession()->SetUser(nullptr);
        SetLoggedIn(false);
        Redirect("/");

        // We already sent a reply
        return PAGE_DONE;
    } else if (sURI == "/login") {
        if (GetParam("submitted").ToBool()) {
            m_sUser = GetParam("user");
            m_sPass = GetParam("pass");
            m_bLoggedIn = OnLogin(m_sUser, m_sPass, false);

            // AcceptedLogin()/RefusedLogin() will call Redirect()
            return PAGE_DEFERRED;
        }

        Redirect("/");  // the login form is here
        return PAGE_DONE;
    } else if (sURI.StartsWith("/pub/")) {
        return PrintStaticFile(sURI, sPageRet);
    } else if (sURI.StartsWith("/skinfiles/")) {
        CString sSkinName = sURI.substr(11);
        CString::size_type uPathStart = sSkinName.find("/");
        if (uPathStart != CString::npos) {
            CString sFilePath = sSkinName.substr(uPathStart + 1);
            sSkinName.erase(uPathStart);

            m_Template.ClearPaths();
            m_Template.AppendPath(GetSkinPath(sSkinName) + "pub");

            if (PrintFile(m_Template.ExpandFile(sFilePath))) {
                return PAGE_DONE;
            } else {
                return PAGE_NOTFOUND;
            }
        }
        return PAGE_NOTFOUND;
    } else if (sURI.StartsWith("/mods/") || sURI.StartsWith("/modfiles/")) {
        // Make sure modules are treated as directories
        if (!sURI.EndsWith("/") && !sURI.Contains(".") &&
            !sURI.TrimLeft_n("/mods/").TrimLeft_n("/").Contains("/")) {
            Redirect(sURI + "/");
            return PAGE_DONE;
        }

        // The URI looks like:
        // /mods/[type]/([network]/)?[module][/page][?arg1=val1&arg2=val2...]

        m_sPath = GetPath().TrimLeft_n("/");

        m_sPath.TrimPrefix("mods/");
        m_sPath.TrimPrefix("modfiles/");

        CString sType = m_sPath.Token(0, false, "/");
        m_sPath = m_sPath.Token(1, true, "/");

        CModInfo::EModuleType eModType;
        if (sType.Equals("global")) {
            eModType = CModInfo::GlobalModule;
        } else if (sType.Equals("user")) {
            eModType = CModInfo::UserModule;
        } else if (sType.Equals("network")) {
            eModType = CModInfo::NetworkModule;
        } else {
            PrintErrorPage(403, "Forbidden",
                           "Unknown module type [" + sType + "]");
            return PAGE_DONE;
        }

        if ((eModType != CModInfo::GlobalModule) && !ForceLogin()) {
            // Make sure we have a valid user
            return PAGE_DONE;
        }

        CIRCNetwork* pNetwork = nullptr;
        if (eModType == CModInfo::NetworkModule) {
            CString sNetwork = m_sPath.Token(0, false, "/");
            m_sPath = m_sPath.Token(1, true, "/");

            pNetwork = GetSession()->GetUser()->FindNetwork(sNetwork);

            if (!pNetwork) {
                PrintErrorPage(404, "Not Found",
                               "Network [" + sNetwork + "] not found.");
                return PAGE_DONE;
            }
        }

        m_sModName = m_sPath.Token(0, false, "/");
        m_sPage = m_sPath.Token(1, true, "/");

        if (m_sPage.empty()) {
            m_sPage = "index";
        }

        DEBUG("Path [" + m_sPath + "], Module [" + m_sModName + "], Page [" +
              m_sPage + "]");

        CModule* pModule = nullptr;

        switch (eModType) {
            case CModInfo::GlobalModule:
                pModule = CZNC::Get().GetModules().FindModule(m_sModName);
                break;
            case CModInfo::UserModule:
                pModule = GetSession()->GetUser()->GetModules().FindModule(
                    m_sModName);
                break;
            case CModInfo::NetworkModule:
                pModule = pNetwork->GetModules().FindModule(m_sModName);
                break;
        }

        if (!pModule) return PAGE_NOTFOUND;

        // Pass CSRF check to module.
        // Note that the normal CSRF checks are not applied to /mods/ URLs.
        if (IsPost() && !m_bBasicAuth &&
            !pModule->ValidateWebRequestCSRFCheck(*this, m_sPage)) {
            DEBUG("Expected _CSRF_Check: " << GetCSRFCheck());
            DEBUG("Actual _CSRF_Check:   " << GetParam("_CSRF_Check"));
            PrintErrorPage(
                403, "Access denied",
                "POST requests need to send "
                "a secret token to prevent cross-site request forgery attacks.");
            return PAGE_DONE;
        }

        m_Template["ModPath"] = pModule->GetWebPath();
        m_Template["ModFilesPath"] = pModule->GetWebFilesPath();

        if (pModule->WebRequiresLogin() && !ForceLogin()) {
            return PAGE_PRINT;
        } else if (pModule->WebRequiresAdmin() && !GetSession()->IsAdmin()) {
            PrintErrorPage(403, "Forbidden",
                           "You need to be an admin to access this module");
            return PAGE_DONE;
        } else if (pModule->GetType() != CModInfo::GlobalModule &&
                   pModule->GetUser() != GetSession()->GetUser()) {
            PrintErrorPage(403, "Forbidden",
                           "You must login as " +
                               pModule->GetUser()->GetUserName() +
                               " in order to view this page");
            return PAGE_DONE;
        } else if (pModule->OnWebPreRequest(*this, m_sPage)) {
            return PAGE_DEFERRED;
        }

        VWebSubPages& vSubPages = pModule->GetSubPages();

        for (TWebSubPage& SubPage : vSubPages) {
            bool bActive = (m_sModName == pModule->GetModName() &&
                            m_sPage == SubPage->GetName());

            if (bActive && SubPage->RequiresAdmin() &&
                !GetSession()->IsAdmin()) {
                PrintErrorPage(403, "Forbidden",
                               "You need to be an admin to access this page");
                return PAGE_DONE;
            }
        }

        if (pModule && pModule->GetType() != CModInfo::GlobalModule &&
            (!IsLoggedIn() || pModule->GetUser() != GetSession()->GetUser())) {
            AddModLoop("UserModLoop", *pModule);
        }

        if (sURI.StartsWith("/modfiles/")) {
            m_Template.AppendPath(GetSkinPath(GetSkinName()) + "/mods/" +
                                  m_sModName + "/files/");
            m_Template.AppendPath(pModule->GetModDataDir() + "/files/");

            if (PrintFile(m_Template.ExpandFile(m_sPage.TrimLeft_n("/")))) {
                return PAGE_PRINT;
            } else {
                return PAGE_NOTFOUND;
            }
        } else {
            SetPaths(pModule, true);

            CTemplate& breadModule = m_Template.AddRow("BreadCrumbs");
            breadModule["Text"] = pModule->GetModName();
            breadModule["URL"] = pModule->GetWebPath();

            /* if a module returns false from OnWebRequest, it does not
               want the template to be printed, usually because it did a
               redirect. */
            if (pModule->OnWebRequest(*this, m_sPage, m_Template)) {
                // If they already sent a reply, let's assume
                // they did what they wanted to do.
                if (SentHeader()) {
                    return PAGE_DONE;
                }
                return PrintTemplate(m_sPage, sPageRet, pModule);
            }

            if (!SentHeader()) {
                PrintErrorPage(
                    404, "Not Implemented",
                    "The requested module does not acknowledge web requests");
            }
            return PAGE_DONE;
        }
    } else {
        CString sPage(sURI.Trim_n("/"));
        if (sPage.length() < 32) {
            for (unsigned int a = 0; a < sPage.length(); a++) {
                unsigned char c = sPage[a];

                if ((c < '0' || c > '9') && (c < 'a' || c > 'z') &&
                    (c < 'A' || c > 'Z') && c != '_') {
                    return PAGE_NOTFOUND;
                }
            }

            return PrintTemplate(sPage, sPageRet);
        }
    }

    return PAGE_NOTFOUND;
}