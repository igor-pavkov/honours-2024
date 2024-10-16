    ExifData::const_iterator serialNumber(const ExifData& ed)
    {
        static const char* keys[] = {
            "Exif.Image.CameraSerialNumber",
            "Exif.Canon.SerialNumber",
            "Exif.Nikon3.SerialNumber",
            "Exif.Nikon3.SerialNO",
            "Exif.Fujifilm.SerialNumber",
            "Exif.Olympus.SerialNumber2",
            "Exif.Sigma.SerialNumber"
        };
        return findMetadatum(ed, keys, EXV_COUNTOF(keys));
    }