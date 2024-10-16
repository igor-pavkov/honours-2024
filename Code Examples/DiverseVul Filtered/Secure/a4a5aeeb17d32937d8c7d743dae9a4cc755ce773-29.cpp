void CWebSock::GetAvailSkins(VCString& vRet) const {
    vRet.clear();

    CString sRoot(GetSkinPath("_default_"));

    sRoot.TrimRight("/");
    sRoot.TrimRight("_default_");
    sRoot.TrimRight("/");

    if (!sRoot.empty()) {
        sRoot += "/";
    }

    if (!sRoot.empty() && CFile::IsDir(sRoot)) {
        CDir Dir(sRoot);

        for (const CFile* pSubDir : Dir) {
            if (pSubDir->IsDir() && pSubDir->GetShortName() == "_default_") {
                vRet.push_back(pSubDir->GetShortName());
                break;
            }
        }

        for (const CFile* pSubDir : Dir) {
            if (pSubDir->IsDir() && pSubDir->GetShortName() != "_default_" &&
                pSubDir->GetShortName() != ".svn") {
                vRet.push_back(pSubDir->GetShortName());
            }
        }
    }
}