    void CrwMap::encode0x0805(const Image&      image,
                              const CrwMapping* pCrwMapping,
                                    CiffHeader* pHead)
    {
        assert(pCrwMapping != 0);
        assert(pHead != 0);

        std::string comment = image.comment();

        CiffComponent* cc = pHead->findComponent(pCrwMapping->crwTagId_,
                                                 pCrwMapping->crwDir_);
        if (!comment.empty()) {
            uint32_t size = static_cast<uint32_t>(comment.size());
            if (cc && cc->size() > size) size = cc->size();
            DataBuf buf(size);
            std::memset(buf.pData_, 0x0, buf.size_);
            std::memcpy(buf.pData_, comment.data(), comment.size());
            pHead->add(pCrwMapping->crwTagId_, pCrwMapping->crwDir_, buf);
        }
        else {
            if (cc) {
                // Just delete the value, do not remove the tag
                DataBuf buf(cc->size());
                std::memset(buf.pData_, 0x0, buf.size_);
                cc->setValue(buf);
            }
        }
    } // CrwMap::encode0x0805