CWebSock::CWebSock(const CString& sURIPrefix)
    : CHTTPSock(nullptr, sURIPrefix),
      m_bPathsSet(false),
      m_Template(),
      m_spAuth(),
      m_sModName(""),
      m_sPath(""),
      m_sPage(""),
      m_spSession() {
    m_Template.AddTagHandler(std::make_shared<CZNCTagHandler>(*this));
}