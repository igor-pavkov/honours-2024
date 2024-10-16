MessageCompressorManager& MessageCompressorManager::forSession(
    const transport::SessionHandle& session) {
    return getForSession(session.get());
}