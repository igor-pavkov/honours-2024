void CWebSession::FillMessageLoops(CTemplate& Tmpl) {
    for (const CString& sMessage : m_vsErrorMsgs) {
        CTemplate& Row = Tmpl.AddRow("ErrorLoop");
        Row["Message"] = sMessage;
    }

    for (const CString& sMessage : m_vsSuccessMsgs) {
        CTemplate& Row = Tmpl.AddRow("SuccessLoop");
        Row["Message"] = sMessage;
    }
}