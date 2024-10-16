    void CrwMap::decode0x180e(const CiffComponent& ciffComponent,
                              const CrwMapping*    pCrwMapping,
                                    Image&         image,
                                    ByteOrder      byteOrder)
    {
        if (ciffComponent.size() < 8 || ciffComponent.typeId() != unsignedLong) {
            return decodeBasic(ciffComponent, pCrwMapping, image, byteOrder);
        }
        assert(pCrwMapping != 0);
        ULongValue v;
        v.read(ciffComponent.pData(), 8, byteOrder);
        time_t t = v.value_[0];
#ifdef EXV_HAVE_GMTIME_R
        struct tm tms;
        struct tm* tm = &tms;
        tm = gmtime_r(&t, tm);
#else
        struct tm* tm = std::gmtime(&t);
#endif
        if (tm) {
            const size_t m = 20;
            char s[m];
            std::strftime(s, m, "%Y:%m:%d %H:%M:%S", tm);

            ExifKey key(pCrwMapping->tag_, Internal::groupName(pCrwMapping->ifdId_));
            AsciiValue value;
            value.read(std::string(s));
            image.exifData().add(key, &value);
        }
    } // CrwMap::decode0x180e