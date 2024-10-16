CWebAuth::CWebAuth(CWebSock* pWebSock, const CString& sUsername,
                   const CString& sPassword, bool bBasic)
    : CAuthBase(sUsername, sPassword, pWebSock),
      m_pWebSock(pWebSock),
      m_bBasic(bBasic) {}