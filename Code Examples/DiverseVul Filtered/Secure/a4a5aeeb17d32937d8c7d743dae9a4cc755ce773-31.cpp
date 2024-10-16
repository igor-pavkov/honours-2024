std::shared_ptr<CWebSession> CWebSock::GetSession() {
    if (m_spSession) {
        return m_spSession;
    }

    const CString sCookieSessionId = GetRequestCookie("SessionId");
    std::shared_ptr<CWebSession>* pSession =
        Sessions.m_mspSessions.GetItem(sCookieSessionId);

    if (pSession != nullptr) {
        // Refresh the timeout
        Sessions.m_mspSessions.AddItem((*pSession)->GetId(), *pSession);
        (*pSession)->UpdateLastActive();
        m_spSession = *pSession;
        DEBUG("Found existing session from cookie: [" + sCookieSessionId +
              "] IsLoggedIn(" +
              CString((*pSession)->IsLoggedIn()
                          ? "true, " + ((*pSession)->GetUser()->GetUserName())
                          : "false") +
              ")");
        return *pSession;
    }

    if (Sessions.m_mIPSessions.count(GetRemoteIP()) > m_uiMaxSessions) {
        pair<mIPSessionsIterator, mIPSessionsIterator> p =
            Sessions.m_mIPSessions.equal_range(GetRemoteIP());
        mIPSessionsIterator it =
            std::min_element(p.first, p.second, compareLastActive);
        DEBUG("Remote IP:   " << GetRemoteIP() << "; discarding session ["
                              << it->second->GetId() << "]");
        Sessions.m_mspSessions.RemItem(it->second->GetId());
    }

    CString sSessionID;
    do {
        sSessionID = CString::RandomString(32);
        sSessionID += ":" + GetRemoteIP() + ":" + CString(GetRemotePort());
        sSessionID += ":" + GetLocalIP() + ":" + CString(GetLocalPort());
        sSessionID += ":" + CString(time(nullptr));
        sSessionID = sSessionID.SHA256();

        DEBUG("Auto generated session: [" + sSessionID + "]");
    } while (Sessions.m_mspSessions.HasItem(sSessionID));

    std::shared_ptr<CWebSession> spSession(
        new CWebSession(sSessionID, GetRemoteIP()));
    Sessions.m_mspSessions.AddItem(spSession->GetId(), spSession);

    m_spSession = spSession;

    return spSession;
}