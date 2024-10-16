bool CZNCTagHandler::HandleTag(CTemplate& Tmpl, const CString& sName,
                               const CString& sArgs, CString& sOutput) {
    if (sName.Equals("URLPARAM")) {
        // sOutput = CZNC::Get()
        sOutput = m_WebSock.GetParam(sArgs.Token(0), false);
        return true;
    }

    return false;
}