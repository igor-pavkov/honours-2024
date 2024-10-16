    void CiffDirectory::doRemove(CrwDirs& crwDirs, uint16_t crwTagId)
    {
        const Components::iterator b = components_.begin();
        const Components::iterator e = components_.end();
        Components::iterator i;

        if (!crwDirs.empty()) {
            CrwSubDir csd = crwDirs.top();
            crwDirs.pop();
            // Find the directory
            for (i = b; i != e; ++i) {
                if ((*i)->tag() == csd.crwDir_) {
                    // Recursive call to next lower level directory
                    (*i)->remove(crwDirs, crwTagId);
                    if ((*i)->empty()) components_.erase(i);
                    break;
                }
            }
        }
        else {
            // Find the tag
            for (i = b; i != e; ++i) {
                if ((*i)->tagId() == crwTagId) {
                    // Remove the entry and abort the loop
                    delete *i;
                    components_.erase(i);
                    break;
                }
            }
        }
    } // CiffDirectory::doRemove