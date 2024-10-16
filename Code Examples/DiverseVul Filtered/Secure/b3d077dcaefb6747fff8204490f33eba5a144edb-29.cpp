    CiffComponent* CiffComponent::doFindComponent(uint16_t crwTagId,
                                                  uint16_t crwDir) const
    {
        if (tagId() == crwTagId && dir() == crwDir) {
            return const_cast<CiffComponent*>(this);
        }
        return 0;
    } // CiffComponent::doFindComponent