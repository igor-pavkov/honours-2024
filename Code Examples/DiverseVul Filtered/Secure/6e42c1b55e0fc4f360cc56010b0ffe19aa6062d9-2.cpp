    ExifData::const_iterator sceneCaptureType(const ExifData& ed)
    {
        static const char* keys[] = {
            "Exif.Photo.SceneCaptureType",
            "Exif.Olympus.SpecialMode"
        };
        return findMetadatum(ed, keys, EXV_COUNTOF(keys));
    }