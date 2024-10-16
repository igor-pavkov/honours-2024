    void CrwMap::loadStack(CrwDirs& crwDirs, uint16_t crwDir)
    {
        for (int i = 0; crwSubDir_[i].crwDir_ != 0xffff; ++i) {
            if (crwSubDir_[i].crwDir_ == crwDir) {
                crwDirs.push(crwSubDir_[i]);
                crwDir = crwSubDir_[i].parent_;
            }
        }
    } // CrwMap::loadStack