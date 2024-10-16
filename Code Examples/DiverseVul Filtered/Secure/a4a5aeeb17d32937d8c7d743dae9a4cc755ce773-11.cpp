void CWebSock::SetPaths(CModule* pModule, bool bIsTemplate) {
    m_Template.ClearPaths();

    VCString vsDirs = GetDirs(pModule, bIsTemplate);
    for (const CString& sDir : vsDirs) {
        m_Template.AppendPath(sDir);
    }

    m_bPathsSet = true;
}