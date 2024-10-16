bool CWebSock::SendCookie(const CString& sKey, const CString& sValue) {
    const CString sPrefixedKey = CString(GetLocalPort()) + "-" + sKey;

    if (!m_sModName.empty()) {
        return CHTTPSock::SendCookie("Mod-" + m_sModName + "-" + sPrefixedKey,
                                     sValue);
    }

    return CHTTPSock::SendCookie(sPrefixedKey, sValue);
}