    ExifData::const_iterator imageQuality(const ExifData& ed)
    {
        static const char* keys[] = {
            "Exif.CanonCs.Quality",
            "Exif.Fujifilm.Quality",
            "Exif.Sigma.Quality",
            "Exif.Nikon1.Quality",
            "Exif.Nikon2.Quality",
            "Exif.Nikon3.Quality",
            "Exif.Olympus.Quality",
            "Exif.OlympusCs.Quality",
            "Exif.Panasonic.Quality",
            "Exif.Minolta.Quality",
            "Exif.MinoltaCsNew.Quality",
            "Exif.MinoltaCsOld.Quality",
            "Exif.MinoltaCs5D.Quality",
            "Exif.MinoltaCs7D.Quality",
            "Exif.Sony1MltCsA100.Quality",
            "Exif.Sony1.JPEGQuality",
            "Exif.Sony1.Quality",
            "Exif.Sony1Cs.Quality",
            "Exif.Sony2.JPEGQuality",
            "Exif.Sony2.Quality",
            "Exif.Sony2Cs.Quality",
            "Exif.Casio.Quality",
            "Exif.Casio2.QualityMode",
            "Exif.Casio2.Quality"
        };
        return findMetadatum(ed, keys, EXV_COUNTOF(keys));
    }