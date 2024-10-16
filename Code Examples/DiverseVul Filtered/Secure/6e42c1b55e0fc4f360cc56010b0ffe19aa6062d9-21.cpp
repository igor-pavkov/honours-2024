    ExifData::const_iterator contrast(const ExifData& ed)
    {
        static const char* keys[] = {
            "Exif.Photo.Contrast",
            "Exif.CanonCs.Contrast",
            "Exif.Fujifilm.Tone",
            "Exif.MinoltaCsNew.Contrast",
            "Exif.MinoltaCsOld.Contrast",
            "Exif.MinoltaCs7D.Contrast",
            "Exif.MinoltaCs5D.Contrast",
            "Exif.Olympus.Contrast",
            "Exif.Panasonic.Contrast",
            "Exif.Pentax.Contrast",
            "Exif.PentaxDng.Contrast",
            "Exif.Sigma.Contrast",
            "Exif.Casio.Contrast",
            "Exif.Casio2.Contrast",
            "Exif.Casio2.Contrast2"

        };
        return findMetadatum(ed, keys, EXV_COUNTOF(keys));
    }