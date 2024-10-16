    void CrwMap::encode0x2008(const Image&      image,
                              const CrwMapping* pCrwMapping,
                                    CiffHeader* pHead)
    {
        assert(pCrwMapping != 0);
        assert(pHead != 0);

        ExifThumbC exifThumb(image.exifData());
        DataBuf buf = exifThumb.copy();
        if (buf.size_ != 0) {
            pHead->add(pCrwMapping->crwTagId_, pCrwMapping->crwDir_, buf);
        }
        else {
            pHead->remove(pCrwMapping->crwTagId_, pCrwMapping->crwDir_);
        }
    } // CrwMap::encode0x2008