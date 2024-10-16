static inline bool compareLastActive(
    const std::pair<const CString, CWebSession*>& first,
    const std::pair<const CString, CWebSession*>& second) {
    return first.second->GetLastActive() < second.second->GetLastActive();
}