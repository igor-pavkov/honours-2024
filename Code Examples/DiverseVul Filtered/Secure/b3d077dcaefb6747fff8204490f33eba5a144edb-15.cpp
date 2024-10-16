    void CiffHeader::remove(uint16_t crwTagId, uint16_t crwDir)
    {
        if (pRootDir_) {
            CrwDirs crwDirs;
            CrwMap::loadStack(crwDirs, crwDir);
            uint16_t rootDirectory = crwDirs.top().crwDir_;
            UNUSED(rootDirectory);
            assert(rootDirectory == 0x0000);
            crwDirs.pop();
            pRootDir_->remove(crwDirs, crwTagId);
        }
    } // CiffHeader::remove