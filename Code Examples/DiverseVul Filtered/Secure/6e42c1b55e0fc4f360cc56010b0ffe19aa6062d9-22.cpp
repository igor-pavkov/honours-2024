    ExifData::const_iterator saturation(const ExifData& ed)
    {
        static const char* keys[] = {
            "Exif.Photo.Saturation",
            "Exif.CanonCs.Saturation",
            "Exif.MinoltaCsNew.Saturation",
            "Exif.MinoltaCsOld.Saturation",
            "Exif.MinoltaCs7D.Saturation",
            "Exif.MinoltaCs5D.Saturation",
            "Exif.Fujifilm.Color",
            "Exif.Nikon3.Saturation",
            "Exif.Panasonic.Saturation",
            "Exif.Pentax.Saturation",
            "Exif.PentaxDng.Saturation",
            "Exif.Sigma.Saturation",
            "Exif.Casio.Saturation",
            "Exif.Casio2.Saturation",
            "Exif.Casio2.Saturation2"
        };
        return findMetadatum(ed, keys, EXV_COUNTOF(keys));
    }