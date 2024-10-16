void CWebAuth::RefusedLogin(const CString& sReason) {
    if (m_pWebSock) {
        std::shared_ptr<CWebSession> spSession = m_pWebSock->GetSession();

        spSession->AddError("Invalid login!");
        spSession->SetUser(nullptr);

        m_pWebSock->SetLoggedIn(false);
        m_pWebSock->UnPauseRead();
        if (m_bBasic) {
            m_pWebSock->AddHeader("WWW-Authenticate", "Basic realm=\"ZNC\"");
            m_pWebSock->CHTTPSock::PrintErrorPage(
                401, "Unauthorized",
                "HTTP Basic authentication attempted with invalid credentials");
            // Why CWebSock makes this function protected?..
        } else {
            m_pWebSock->Redirect("/?cookie_check=true");
        }

        DEBUG("UNSUCCESSFUL login attempt ==> REASON [" + sReason +
              "] ==> SESSION [" + spSession->GetId() + "]");
    }
}