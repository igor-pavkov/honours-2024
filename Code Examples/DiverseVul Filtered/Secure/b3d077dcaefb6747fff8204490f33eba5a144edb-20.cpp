    void CrwMap::encode0x180e(const Image&      image,
                              const CrwMapping* pCrwMapping,
                                    CiffHeader* pHead)
    {
        assert(pCrwMapping != 0);
        assert(pHead != 0);

        time_t t = 0;
        const ExifKey key(pCrwMapping->tag_, Internal::groupName(pCrwMapping->ifdId_));
        const ExifData::const_iterator ed = image.exifData().findKey(key);
        if (ed != image.exifData().end()) {
            struct tm tm;
            std::memset(&tm, 0x0, sizeof(tm));
            int rc = exifTime(ed->toString().c_str(), &tm);
            if (rc == 0) t = timegm(&tm);
        }
        if (t != 0) {
            DataBuf buf(12);
            std::memset(buf.pData_, 0x0, 12);
            ul2Data(buf.pData_, static_cast<uint32_t>(t), pHead->byteOrder());
            pHead->add(pCrwMapping->crwTagId_, pCrwMapping->crwDir_, buf);
        }
        else {
            pHead->remove(pCrwMapping->crwTagId_, pCrwMapping->crwDir_);
        }
    } // CrwMap::encode0x180e