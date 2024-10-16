CString CWebSock::FindTmpl(CModule* pModule, const CString& sName) {
    VCString vsDirs = GetDirs(pModule, true);
    CString sFile = pModule->GetModName() + "_" + sName;
    for (const CString& sDir : vsDirs) {
        if (CFile::Exists(CDir::ChangeDir(sDir, sFile))) {
            m_Template.AppendPath(sDir);
            return sFile;
        }
    }
    return sName;
}