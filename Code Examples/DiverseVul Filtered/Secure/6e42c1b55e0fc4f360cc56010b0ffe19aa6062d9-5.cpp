    ExifData::const_iterator lensName(const ExifData& ed)
    {
        static const char* keys[] = {
            // Exif.Canon.LensModel only reports focal length.
            // Try Exif.CanonCs.LensType first.
            "Exif.CanonCs.LensType",
            "Exif.Photo.LensModel",
            "Exif.NikonLd1.LensIDNumber",
            "Exif.NikonLd2.LensIDNumber",
            "Exif.NikonLd3.LensIDNumber",
            "Exif.Pentax.LensType",
            "Exif.PentaxDng.LensType",
            "Exif.Minolta.LensID",
            "Exif.SonyMinolta.LensID",
            "Exif.Sony1.LensID",
            "Exif.Sony2.LensID",
            "Exif.OlympusEq.LensType",
            "Exif.Panasonic.LensType",
            "Exif.Samsung2.LensType"
        };
        return findMetadatum(ed, keys, EXV_COUNTOF(keys));
    }