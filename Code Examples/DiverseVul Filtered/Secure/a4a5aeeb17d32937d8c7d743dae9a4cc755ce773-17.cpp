CWebSock::EPageReqResult CWebSock::PrintStaticFile(const CString& sPath,
                                                   CString& sPageRet,
                                                   CModule* pModule) {
    SetPaths(pModule);
    CString sFile = m_Template.ExpandFile(sPath.TrimLeft_n("/"));
    DEBUG("About to print [" + sFile + "]");
    // Either PrintFile() fails and sends an error page or it succeeds and
    // sends a result. In both cases we don't have anything more to do.
    PrintFile(sFile);
    return PAGE_DONE;
}