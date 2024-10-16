    ExifData::const_iterator fNumber(const ExifData& ed)
    {
        static const char* keys[] = {
            "Exif.Photo.FNumber",
            "Exif.Image.FNumber",
            "Exif.Samsung2.FNumber"
        };
        return findMetadatum(ed, keys, EXV_COUNTOF(keys));
    }