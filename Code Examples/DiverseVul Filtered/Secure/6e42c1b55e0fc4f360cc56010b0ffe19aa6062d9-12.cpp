    ExifData::const_iterator afPoint(const ExifData& ed)
    {
        static const char* keys[] = {
            "Exif.CanonPi.AFPointsUsed",
            "Exif.CanonPi.AFPointsUsed20D",
            "Exif.CanonSi.AFPointUsed",
            "Exif.CanonCs.AFPoint",
            "Exif.MinoltaCs7D.AFPoints",
            "Exif.Nikon1.AFFocusPos",
            "Exif.NikonAf.AFPoint",
            "Exif.NikonAf.AFPointsInFocus",
            "Exif.NikonAf2.AFPointsUsed",
            "Exif.NikonAf2.PrimaryAFPoint",
            "Exif.OlympusFi.AFPoint",
            "Exif.Pentax.AFPoint",
            "Exif.Pentax.AFPointInFocus",
            "Exif.PentaxDng.AFPoint",
            "Exif.PentaxDng.AFPointInFocus",
            "Exif.Sony1Cs.LocalAFAreaPoint",
            "Exif.Sony2Cs.LocalAFAreaPoint",
            "Exif.Sony1Cs2.LocalAFAreaPoint",
            "Exif.Sony2Cs2.LocalAFAreaPoint",
            "Exif.Sony1MltCsA100.LocalAFAreaPoint",
            "Exif.Casio.AFPoint",
            "Exif.Casio2.AFPointPosition"
        };
        return findMetadatum(ed, keys, EXV_COUNTOF(keys));
    }