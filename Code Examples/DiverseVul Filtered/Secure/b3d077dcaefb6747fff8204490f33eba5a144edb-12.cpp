    void CiffHeader::add(uint16_t crwTagId, uint16_t crwDir, DataBuf buf)
    {
        CrwDirs crwDirs;
        CrwMap::loadStack(crwDirs, crwDir);
        uint16_t rootDirectory = crwDirs.top().crwDir_;
        UNUSED(rootDirectory);
        assert(rootDirectory == 0x0000);
        crwDirs.pop();
        if (!pRootDir_) pRootDir_ = new CiffDirectory;
        if ( pRootDir_) {
            CiffComponent* child = pRootDir_->add(crwDirs, crwTagId);
            if ( child )   child->setValue(buf);
        }
    } // CiffHeader::add