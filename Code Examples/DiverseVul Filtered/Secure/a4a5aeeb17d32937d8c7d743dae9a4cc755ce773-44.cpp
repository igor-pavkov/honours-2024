void CWebSock::PrintErrorPage(const CString& sMessage) {
    m_Template.SetFile("Error.tmpl");

    m_Template["Action"] = "error";
    m_Template["Title"] = "Error";
    m_Template["Error"] = sMessage;
}