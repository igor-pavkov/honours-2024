    void CrwMap::encodeArray(const Image&      image,
                             const CrwMapping* pCrwMapping,
                                   CiffHeader* pHead)
    {
        assert(pCrwMapping != 0);
        assert(pHead != 0);

        IfdId ifdId = ifdIdNotSet;
        switch (pCrwMapping->tag_) {
        case 0x0001: ifdId = canonCsId; break;
        case 0x0004: ifdId = canonSiId; break;
        case 0x000f: ifdId = canonCfId; break;
        case 0x0012: ifdId = canonPiId; break;
        }
        assert(ifdId != ifdIdNotSet);
        DataBuf buf = packIfdId(image.exifData(), ifdId, pHead->byteOrder());
        if (buf.size_ == 0) {
            // Try the undecoded tag
            encodeBasic(image, pCrwMapping, pHead);
        }
        if (buf.size_ > 0) {
            // Write the number of shorts to the beginning of buf
            us2Data(buf.pData_, static_cast<uint16_t>(buf.size_), pHead->byteOrder());
            pHead->add(pCrwMapping->crwTagId_, pCrwMapping->crwDir_, buf);
        }
        else {
            pHead->remove(pCrwMapping->crwTagId_, pCrwMapping->crwDir_);
        }
    } // CrwMap::encodeArray