    void CrwMap::decode0x1810(const CiffComponent& ciffComponent,
                              const CrwMapping*    pCrwMapping,
                                    Image&         image,
                                    ByteOrder      byteOrder)
    {
        if (ciffComponent.typeId() != unsignedLong || ciffComponent.size() < 28) {
            return decodeBasic(ciffComponent, pCrwMapping, image, byteOrder);
        }

        ExifKey key1("Exif.Photo.PixelXDimension");
        ULongValue value1;
        value1.read(ciffComponent.pData(), 4, byteOrder);
        image.exifData().add(key1, &value1);

        ExifKey key2("Exif.Photo.PixelYDimension");
        ULongValue value2;
        value2.read(ciffComponent.pData() + 4, 4, byteOrder);
        image.exifData().add(key2, &value2);

        int32_t r = getLong(ciffComponent.pData() + 12, byteOrder);
        uint16_t o = RotationMap::orientation(r);
        image.exifData()["Exif.Image.Orientation"] = o;

    } // CrwMap::decode0x1810