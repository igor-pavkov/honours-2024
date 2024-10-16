    ExifData::const_iterator make(const ExifData& ed)
    {
        static const char* keys[] = {
            "Exif.Image.Make"
        };
        return findMetadatum(ed, keys, EXV_COUNTOF(keys));
    }