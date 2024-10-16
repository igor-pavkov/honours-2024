void CWebSock::OnPageRequest(const CString& sURI) {
    CString sPageRet;
    EPageReqResult eRet = OnPageRequestInternal(sURI, sPageRet);
    switch (eRet) {
        case PAGE_PRINT:
            PrintPage(sPageRet);
            break;
        case PAGE_DEFERRED:
            // Something else will later call Close()
            break;
        case PAGE_DONE:
            // Redirect or something like that, it's done, just make sure
            // the connection will be closed
            Close(CLT_AFTERWRITE);
            break;
        case PAGE_NOTFOUND:
        default:
            PrintNotFound();
            break;
    }
}