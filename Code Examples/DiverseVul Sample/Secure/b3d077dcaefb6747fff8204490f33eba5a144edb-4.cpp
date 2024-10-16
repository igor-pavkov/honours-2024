    CiffComponent* CiffHeader::findComponent(uint16_t crwTagId,
                                             uint16_t crwDir) const
    {
        if (pRootDir_ == 0) return 0;
        return pRootDir_->findComponent(crwTagId, crwDir);
    } // CiffHeader::findComponent