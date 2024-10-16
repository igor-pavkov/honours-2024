CWebSock::EPageReqResult CWebSock::PrintTemplate(const CString& sPageName,
                                                 CString& sPageRet,
                                                 CModule* pModule) {
    SetVars();

    m_Template["PageName"] = sPageName;

    if (pModule) {
        m_Template["ModName"] = pModule->GetModName();

        if (m_Template.find("Title") == m_Template.end()) {
            m_Template["Title"] = pModule->GetWebMenuTitle();
        }

        std::vector<CTemplate*>* breadcrumbs =
            m_Template.GetLoop("BreadCrumbs");
        if (breadcrumbs->size() == 1 &&
            m_Template["Title"] != pModule->GetModName()) {
            // Module didn't add its own breadcrumbs, so add a generic one...
            // But it'll be useless if it's the same as module name
            CTemplate& bread = m_Template.AddRow("BreadCrumbs");
            bread["Text"] = m_Template["Title"];
        }
    }

    if (!m_bPathsSet) {
        SetPaths(pModule, true);
    }

    if (m_Template.GetFileName().empty() &&
        !m_Template.SetFile(sPageName + ".tmpl")) {
        return PAGE_NOTFOUND;
    }

    if (m_Template.PrintString(sPageRet)) {
        return PAGE_PRINT;
    } else {
        return PAGE_NOTFOUND;
    }
}