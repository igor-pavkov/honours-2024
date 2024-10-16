size_t CWebSession::AddSuccess(const CString& sMessage) {
    m_vsSuccessMsgs.push_back(sMessage);
    return m_vsSuccessMsgs.size();
}