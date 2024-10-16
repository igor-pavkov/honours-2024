    const CrwMapping* CrwMap::crwMapping(uint16_t crwDir, uint16_t crwTagId)
    {
        for (int i = 0; crwMapping_[i].ifdId_ != ifdIdNotSet; ++i) {
            if (   crwMapping_[i].crwDir_ == crwDir
                && crwMapping_[i].crwTagId_ == crwTagId) {
                return &(crwMapping_[i]);
            }
        }
        return 0;
    } // CrwMap::crwMapping