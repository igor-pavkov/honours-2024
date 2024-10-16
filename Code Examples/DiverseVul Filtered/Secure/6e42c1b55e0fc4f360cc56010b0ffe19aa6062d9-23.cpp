    ExifData::const_iterator sceneMode(const ExifData& ed)
    {
        static const char* keys[] = {
            "Exif.CanonCs.EasyMode",
            "Exif.Fujifilm.PictureMode",
            "Exif.MinoltaCsNew.SubjectProgram",
            "Exif.MinoltaCsOld.SubjectProgram",
            "Exif.Minolta.SceneMode",
            "Exif.SonyMinolta.SceneMode",
            "Exif.Sony1.SceneMode",
            "Exif.Sony2.SceneMode",
            "Exif.OlympusCs.SceneMode",
            "Exif.Panasonic.ShootingMode",
            "Exif.Panasonic.SceneMode",
            "Exif.Pentax.PictureMode",
            "Exif.PentaxDng.PictureMode",
            "Exif.Photo.SceneCaptureType"
        };
        return findMetadatum(ed, keys, EXV_COUNTOF(keys));
    }