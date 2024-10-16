    ExifData::const_iterator model(const ExifData& ed)
    {
        static const char* keys[] = {
            "Exif.Image.Model"
        };
        return findMetadatum(ed, keys, EXV_COUNTOF(keys));
    }