    ExifData::const_iterator whiteBalance(const ExifData& ed)
    {
        static const char* keys[] = {
            "Exif.CanonSi.WhiteBalance",
            "Exif.Fujifilm.WhiteBalance",
            "Exif.Sigma.WhiteBalance",
            "Exif.Nikon1.WhiteBalance",
            "Exif.Nikon2.WhiteBalance",
            "Exif.Nikon3.WhiteBalance",
            "Exif.Olympus.WhiteBalance",
            "Exif.OlympusCs.WhiteBalance",
            "Exif.Panasonic.WhiteBalance",
            "Exif.MinoltaCs5D.WhiteBalance",
            "Exif.MinoltaCs7D.WhiteBalance",
            "Exif.MinoltaCsNew.WhiteBalance",
            "Exif.MinoltaCsOld.WhiteBalance",
            "Exif.Minolta.WhiteBalance",
            "Exif.Sony1MltCsA100.WhiteBalance",
            "Exif.SonyMinolta.WhiteBalance",
            "Exif.Sony1.WhiteBalance",
            "Exif.Sony2.WhiteBalance",
            "Exif.Sony1.WhiteBalance2",
            "Exif.Sony2.WhiteBalance2",
            "Exif.Casio.WhiteBalance",
            "Exif.Casio2.WhiteBalance",
            "Exif.Casio2.WhiteBalance2",
            "Exif.Photo.WhiteBalance"
        };
        return findMetadatum(ed, keys, EXV_COUNTOF(keys));
    }