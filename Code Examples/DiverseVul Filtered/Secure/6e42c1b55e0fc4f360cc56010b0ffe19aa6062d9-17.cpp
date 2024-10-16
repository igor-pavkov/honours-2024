    ExifData::const_iterator focalLength(const ExifData& ed)
    {
        static const char* keys[] = {
            "Exif.Photo.FocalLength",
            "Exif.Image.FocalLength",
            "Exif.Canon.FocalLength",
            "Exif.NikonLd2.FocalLength",
            "Exif.NikonLd3.FocalLength",
            "Exif.MinoltaCsNew.FocalLength",
            "Exif.Pentax.FocalLength",
            "Exif.PentaxDng.FocalLength",
            "Exif.Casio2.FocalLength"
        };
        return findMetadatum(ed, keys, EXV_COUNTOF(keys));
    }