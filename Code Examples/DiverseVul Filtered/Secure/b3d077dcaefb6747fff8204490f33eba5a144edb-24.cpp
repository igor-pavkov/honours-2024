    void CrwMap::decode0x2008(const CiffComponent& ciffComponent,
                              const CrwMapping*    /*pCrwMapping*/,
                                    Image&         image,
                                    ByteOrder      /*byteOrder*/)
    {
        ExifThumb exifThumb(image.exifData());
        exifThumb.setJpegThumbnail(ciffComponent.pData(), ciffComponent.size());
    } // CrwMap::decode0x2008