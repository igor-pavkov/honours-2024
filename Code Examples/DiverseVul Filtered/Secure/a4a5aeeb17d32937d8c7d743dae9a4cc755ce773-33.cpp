CWebSock::~CWebSock() {
    if (m_spAuth) {
        m_spAuth->Invalidate();
    }

    // we have to account for traffic here because CSocket does
    // not have a valid CModule* pointer.
    CUser* pUser = GetSession()->GetUser();
    if (pUser) {
        pUser->AddBytesWritten(GetBytesWritten());
        pUser->AddBytesRead(GetBytesRead());
    } else {
        CZNC::Get().AddBytesWritten(GetBytesWritten());
        CZNC::Get().AddBytesRead(GetBytesRead());
    }

    // bytes have been accounted for, so make sure they don't get again:
    ResetBytesWritten();
    ResetBytesRead();
}