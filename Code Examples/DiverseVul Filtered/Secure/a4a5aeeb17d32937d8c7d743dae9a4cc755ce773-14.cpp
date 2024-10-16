CWebSession::CWebSession(const CString& sId, const CString& sIP)
    : m_sId(sId),
      m_sIP(sIP),
      m_pUser(nullptr),
      m_vsErrorMsgs(),
      m_vsSuccessMsgs(),
      m_tmLastActive() {
    Sessions.m_mIPSessions.insert(make_pair(sIP, this));
    UpdateLastActive();
}