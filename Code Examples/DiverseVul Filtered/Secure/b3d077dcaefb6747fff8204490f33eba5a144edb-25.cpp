    void CrwMap::encodeBasic(const Image&      image,
                             const CrwMapping* pCrwMapping,
                                   CiffHeader* pHead)
    {
        assert(pCrwMapping != 0);
        assert(pHead != 0);

        // Determine the source Exif metadatum
        ExifKey ek(pCrwMapping->tag_, Internal::groupName(pCrwMapping->ifdId_));
        ExifData::const_iterator ed = image.exifData().findKey(ek);

        // Set the new value or remove the entry
        if (ed != image.exifData().end()) {
            DataBuf buf(ed->size());
            ed->copy(buf.pData_, pHead->byteOrder());
            pHead->add(pCrwMapping->crwTagId_, pCrwMapping->crwDir_, buf);
        }
        else {
            pHead->remove(pCrwMapping->crwTagId_, pCrwMapping->crwDir_);
        }
    } // CrwMap::encodeBasic