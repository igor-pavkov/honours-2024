CString CWebSock::GetSkinName() {
    std::shared_ptr<CWebSession> spSession = GetSession();

    if (spSession->IsLoggedIn() &&
        !spSession->GetUser()->GetSkinName().empty()) {
        return spSession->GetUser()->GetSkinName();
    }

    return CZNC::Get().GetSkinName();
}