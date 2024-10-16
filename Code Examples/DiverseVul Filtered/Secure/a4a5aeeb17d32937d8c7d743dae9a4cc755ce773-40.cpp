bool CWebSock::OnLogin(const CString& sUser, const CString& sPass,
                       bool bBasic) {
    DEBUG("=================== CWebSock::OnLogin(), basic auth? "
          << std::boolalpha << bBasic);
    m_spAuth = std::make_shared<CWebAuth>(this, sUser, sPass, bBasic);

    // Some authentication module could need some time, block this socket
    // until then. CWebAuth will UnPauseRead().
    PauseRead();
    CZNC::Get().AuthUser(m_spAuth);

    // If CWebAuth already set this, don't change it.
    return IsLoggedIn();
}