    ExifData::const_iterator subjectDistance(const ExifData& ed)
    {
        static const char* keys[] = {
            "Exif.Photo.SubjectDistance",
            "Exif.Image.SubjectDistance",
            "Exif.CanonSi.SubjectDistance",
            "Exif.CanonFi.FocusDistanceUpper",
            "Exif.CanonFi.FocusDistanceLower",
            "Exif.MinoltaCsNew.FocusDistance",
            "Exif.Nikon1.FocusDistance",
            "Exif.Nikon3.FocusDistance",
            "Exif.NikonLd2.FocusDistance",
            "Exif.NikonLd3.FocusDistance",
            "Exif.Olympus.FocusDistance",
            "Exif.OlympusFi.FocusDistance",
            "Exif.Casio.ObjectDistance",
            "Exif.Casio2.ObjectDistance"
        };
        return findMetadatum(ed, keys, EXV_COUNTOF(keys));
    }