    ExifData::const_iterator exposureTime(const ExifData& ed)
    {
        static const char* keys[] = {
            "Exif.Photo.ExposureTime",
            "Exif.Image.ExposureTime",
            "Exif.Samsung2.ExposureTime"
        };
        return findMetadatum(ed, keys, EXV_COUNTOF(keys));
    }