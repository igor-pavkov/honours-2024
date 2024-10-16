    ExifData::const_iterator macroMode(const ExifData& ed)
    {
        static const char* keys[] = {
            "Exif.CanonCs.Macro",
            "Exif.Fujifilm.Macro",
            "Exif.Olympus.Macro",
            "Exif.OlympusCs.MacroMode",
            "Exif.Panasonic.Macro",
            "Exif.MinoltaCsNew.MacroMode",
            "Exif.MinoltaCsOld.MacroMode",
            "Exif.Sony1.Macro",
            "Exif.Sony2.Macro"
        };
        return findMetadatum(ed, keys, EXV_COUNTOF(keys));
    }