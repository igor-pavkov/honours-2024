void CWebSock::SetVars() {
    m_Template["SessionUser"] = GetUser();
    m_Template["SessionIP"] = GetRemoteIP();
    m_Template["Tag"] = CZNC::GetTag(GetSession()->GetUser() != nullptr, true);
    m_Template["Version"] = CZNC::GetVersion();
    m_Template["SkinName"] = GetSkinName();
    m_Template["_CSRF_Check"] = GetCSRFCheck();
    m_Template["URIPrefix"] = GetURIPrefix();

    if (GetSession()->IsAdmin()) {
        m_Template["IsAdmin"] = "true";
    }

    GetSession()->FillMessageLoops(m_Template);
    GetSession()->ClearMessageLoops();

    // Global Mods
    CModules& vgMods = CZNC::Get().GetModules();
    for (CModule* pgMod : vgMods) {
        AddModLoop("GlobalModLoop", *pgMod);
    }

    // User Mods
    if (IsLoggedIn()) {
        CModules& vMods = GetSession()->GetUser()->GetModules();

        for (CModule* pMod : vMods) {
            AddModLoop("UserModLoop", *pMod);
        }

        vector<CIRCNetwork*> vNetworks = GetSession()->GetUser()->GetNetworks();
        for (CIRCNetwork* pNetwork : vNetworks) {
            CModules& vnMods = pNetwork->GetModules();

            CTemplate& Row = m_Template.AddRow("NetworkModLoop");
            Row["NetworkName"] = pNetwork->GetName();

            for (CModule* pnMod : vnMods) {
                AddModLoop("ModLoop", *pnMod, &Row);
            }
        }
    }

    if (IsLoggedIn()) {
        m_Template["LoggedIn"] = "true";
    }
}