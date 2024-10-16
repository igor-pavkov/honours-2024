    ExifData::const_iterator flashBias(const ExifData& ed)
    {
        static const char* keys[] = {
            "Exif.CanonSi.FlashBias",
            "Exif.Panasonic.FlashBias",
            "Exif.Olympus.FlashBias",
            "Exif.OlympusCs.FlashExposureComp",
            "Exif.Minolta.FlashExposureComp",
            "Exif.SonyMinolta.FlashExposureComp",
            "Exif.Sony1.FlashExposureComp",
            "Exif.Sony2.FlashExposureComp"
        };
        return findMetadatum(ed, keys, EXV_COUNTOF(keys));
    }