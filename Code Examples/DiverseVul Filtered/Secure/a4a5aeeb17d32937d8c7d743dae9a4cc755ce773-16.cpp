bool CWebSock::ValidateCSRFCheck(const CString& sURI) {
    return sURI == "/login" || GetParam("_CSRF_Check") == GetCSRFCheck();
}