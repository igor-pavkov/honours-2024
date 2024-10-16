void CWebAuth::Invalidate() {
    CAuthBase::Invalidate();
    m_pWebSock = nullptr;
}