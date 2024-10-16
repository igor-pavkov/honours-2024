    ExifData::const_iterator sharpness(const ExifData& ed)
    {
        static const char* keys[] = {
            "Exif.Photo.Sharpness",
            "Exif.CanonCs.Sharpness",
            "Exif.Fujifilm.Sharpness",
            "Exif.MinoltaCsNew.Sharpness",
            "Exif.MinoltaCsOld.Sharpness",
            "Exif.MinoltaCs7D.Sharpness",
            "Exif.MinoltaCs5D.Sharpness",
            "Exif.Olympus.SharpnessFactor",
            "Exif.Panasonic.Sharpness",
            "Exif.Pentax.Sharpness",
            "Exif.PentaxDng.Sharpness",
            "Exif.Sigma.Sharpness",
            "Exif.Casio.Sharpness",
            "Exif.Casio2.Sharpness",
            "Exif.Casio2.Sharpness2"
        };
        return findMetadatum(ed, keys, EXV_COUNTOF(keys));
    }