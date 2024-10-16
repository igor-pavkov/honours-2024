size_t CWebSession::AddError(const CString& sMessage) {
    m_vsErrorMsgs.push_back(sMessage);
    return m_vsErrorMsgs.size();
}