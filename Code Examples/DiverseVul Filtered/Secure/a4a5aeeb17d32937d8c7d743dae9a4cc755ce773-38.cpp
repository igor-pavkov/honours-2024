CString CWebSock::GetCSRFCheck() {
    std::shared_ptr<CWebSession> pSession = GetSession();
    return pSession->GetId().MD5();
}