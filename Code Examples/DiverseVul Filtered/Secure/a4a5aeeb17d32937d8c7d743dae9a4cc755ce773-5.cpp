Csock* CWebSock::GetSockObj(const CString& sHost, unsigned short uPort) {
    // All listening is done by CListener, thus CWebSock should never have
    // to listen, but since GetSockObj() is pure virtual...
    DEBUG("CWebSock::GetSockObj() called - this should never happen!");
    return nullptr;
}