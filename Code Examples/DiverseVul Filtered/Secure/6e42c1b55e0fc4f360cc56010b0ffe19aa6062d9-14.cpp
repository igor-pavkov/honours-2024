    ExifData::const_iterator exposureMode(const ExifData& ed)
    {
        static const char* keys[] = {
            "Exif.Photo.ExposureProgram",
            "Exif.Image.ExposureProgram",
            "Exif.CanonCs.ExposureProgram",
            "Exif.MinoltaCs7D.ExposureMode",
            "Exif.MinoltaCs5D.ExposureMode",
            "Exif.MinoltaCsNew.ExposureMode",
            "Exif.MinoltaCsOld.ExposureMode",
            "Exif.Sony1MltCsA100.ExposureMode",
            "Exif.Sony1Cs.ExposureProgram",
            "Exif.Sony2Cs.ExposureProgram",
            "Exif.Sigma.ExposureMode"
        };
        return findMetadatum(ed, keys, EXV_COUNTOF(keys));
    }