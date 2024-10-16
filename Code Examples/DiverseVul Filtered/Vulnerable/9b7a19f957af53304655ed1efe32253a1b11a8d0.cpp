    void CrwMap::encode0x1810(const Image&      image,
                              const CrwMapping* pCrwMapping,
                                    CiffHeader* pHead)
    {
        assert(pCrwMapping != 0);
        assert(pHead != 0);

        const ExifKey kX("Exif.Photo.PixelXDimension");
        const ExifKey kY("Exif.Photo.PixelYDimension");
        const ExifKey kO("Exif.Image.Orientation");
        const ExifData &exivData = image.exifData();
        const ExifData::const_iterator edX = exivData.findKey(kX);
        const ExifData::const_iterator edY = exivData.findKey(kY);
        const ExifData::const_iterator edO = exivData.findKey(kO);
        const ExifData::const_iterator edEnd = exivData.end();

        CiffComponent* cc = pHead->findComponent(pCrwMapping->crwTagId_,
                                                 pCrwMapping->crwDir_);
        if (edX != edEnd || edY != edEnd || edO != edEnd) {
            uint32_t size = 28;
            if (cc && cc->size() > size) size = cc->size();
            DataBuf buf(size);
            std::memset(buf.pData_, 0x0, buf.size_);
            if (cc) std::memcpy(buf.pData_ + 8, cc->pData() + 8, cc->size() - 8);
            if (edX != edEnd && edX->size() == 4) {
                edX->copy(buf.pData_, pHead->byteOrder());
            }
            if (edY != edEnd && edY->size() == 4) {
                edY->copy(buf.pData_ + 4, pHead->byteOrder());
            }
            int32_t d = 0;
            if (edO != edEnd && edO->count() > 0 && edO->typeId() == unsignedShort) {
                d = RotationMap::degrees(static_cast<uint16_t>(edO->toLong()));
            }
            l2Data(buf.pData_ + 12, d, pHead->byteOrder());
            pHead->add(pCrwMapping->crwTagId_, pCrwMapping->crwDir_, buf);
        }
        else {
            pHead->remove(pCrwMapping->crwTagId_, pCrwMapping->crwDir_);
        }
    } // CrwMap::encode0x1810