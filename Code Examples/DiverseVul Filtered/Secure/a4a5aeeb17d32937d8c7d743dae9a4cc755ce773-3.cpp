VCString CWebSock::GetDirs(CModule* pModule, bool bIsTemplate) {
    CString sHomeSkinsDir(CZNC::Get().GetZNCPath() + "/webskins/");
    CString sSkinName(GetSkinName());
    VCString vsResult;

    // Module specific paths

    if (pModule) {
        const CString& sModName(pModule->GetModName());

        // 1. ~/.znc/webskins/<user_skin_setting>/mods/<mod_name>/
        //
        if (!sSkinName.empty()) {
            vsResult.push_back(GetSkinPath(sSkinName) + "/mods/" + sModName +
                               "/");
        }

        // 2. ~/.znc/webskins/_default_/mods/<mod_name>/
        //
        vsResult.push_back(GetSkinPath("_default_") + "/mods/" + sModName +
                           "/");

        // 3. ./modules/<mod_name>/tmpl/
        //
        vsResult.push_back(pModule->GetModDataDir() + "/tmpl/");

        // 4. ~/.znc/webskins/<user_skin_setting>/mods/<mod_name>/
        //
        if (!sSkinName.empty()) {
            vsResult.push_back(GetSkinPath(sSkinName) + "/mods/" + sModName +
                               "/");
        }

        // 5. ~/.znc/webskins/_default_/mods/<mod_name>/
        //
        vsResult.push_back(GetSkinPath("_default_") + "/mods/" + sModName +
                           "/");
    }

    // 6. ~/.znc/webskins/<user_skin_setting>/
    //
    if (!sSkinName.empty()) {
        vsResult.push_back(GetSkinPath(sSkinName) +
                           CString(bIsTemplate ? "/tmpl/" : "/"));
    }

    // 7. ~/.znc/webskins/_default_/
    //
    vsResult.push_back(GetSkinPath("_default_") +
                       CString(bIsTemplate ? "/tmpl/" : "/"));

    return vsResult;
}