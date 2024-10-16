    ExifData::const_iterator orientation(const ExifData& ed)
    {
        static const char* keys[] = {
            "Exif.Image.Orientation",
            "Exif.Panasonic.Rotation",
            "Exif.MinoltaCs5D.Rotation",
            "Exif.MinoltaCs5D.Rotation2",
            "Exif.MinoltaCs7D.Rotation",
            "Exif.Sony1MltCsA100.Rotation",
            "Exif.Sony1Cs.Rotation",
            "Exif.Sony2Cs.Rotation",
            "Exif.Sony1Cs2.Rotation",
            "Exif.Sony2Cs2.Rotation",
            "Exif.Sony1MltCsA100.Rotation"
        };
        return findMetadatum(ed, keys, EXV_COUNTOF(keys));
    }