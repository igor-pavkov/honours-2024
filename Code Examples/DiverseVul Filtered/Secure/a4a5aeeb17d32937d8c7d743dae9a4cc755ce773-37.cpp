CString CWebSock::GetRequestCookie(const CString& sKey) {
    const CString sPrefixedKey = CString(GetLocalPort()) + "-" + sKey;
    CString sRet;

    if (!m_sModName.empty()) {
        sRet = CHTTPSock::GetRequestCookie("Mod-" + m_sModName + "-" +
                                           sPrefixedKey);
    }

    if (sRet.empty()) {
        return CHTTPSock::GetRequestCookie(sPrefixedKey);
    }

    return sRet;
}