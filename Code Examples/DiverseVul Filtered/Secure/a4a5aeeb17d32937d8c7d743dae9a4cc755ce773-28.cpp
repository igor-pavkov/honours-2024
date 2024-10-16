void CWebAuth::AcceptedLogin(CUser& User) {
    if (m_pWebSock) {
        std::shared_ptr<CWebSession> spSession = m_pWebSock->GetSession();

        spSession->SetUser(&User);

        m_pWebSock->SetLoggedIn(true);
        m_pWebSock->UnPauseRead();
        if (m_bBasic) {
            m_pWebSock->ReadLine("");
        } else {
            m_pWebSock->Redirect("/?cookie_check=true");
        }

        DEBUG("Successful login attempt ==> USER [" + User.GetUserName() +
              "] ==> SESSION [" + spSession->GetId() + "]");
    }
}