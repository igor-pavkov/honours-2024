CWebSession::~CWebSession() {
    // Find our entry in mIPSessions
    pair<mIPSessionsIterator, mIPSessionsIterator> p =
        Sessions.m_mIPSessions.equal_range(m_sIP);
    mIPSessionsIterator it = p.first;
    mIPSessionsIterator end = p.second;

    while (it != end) {
        if (it->second == this) {
            Sessions.m_mIPSessions.erase(it++);
        } else {
            ++it;
        }
    }
}