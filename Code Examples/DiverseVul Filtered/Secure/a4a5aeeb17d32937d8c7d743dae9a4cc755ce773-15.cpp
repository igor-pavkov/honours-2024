void CWebSessionMap::FinishUserSessions(const CUser& User) {
    iterator it = m_mItems.begin();

    while (it != m_mItems.end()) {
        if (it->second.second->GetUser() == &User) {
            m_mItems.erase(it++);
        } else {
            ++it;
        }
    }
}