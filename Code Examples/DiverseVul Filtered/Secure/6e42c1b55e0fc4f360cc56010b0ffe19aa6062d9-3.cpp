    ExifData::const_iterator meteringMode(const ExifData& ed)
    {
        static const char* keys[] = {
            "Exif.Photo.MeteringMode",
            "Exif.Image.MeteringMode",
            "Exif.CanonCs.MeteringMode",
            "Exif.Sony1MltCsA100.MeteringMode"
        };
        return findMetadatum(ed, keys, EXV_COUNTOF(keys));
    }