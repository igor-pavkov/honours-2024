char const * const * caca_get_dither_algorithm_list(caca_dither_t const *d)
{
    static char const * const list[] =
    {
        "none", "no dithering",
        "ordered2", "2x2 ordered dithering",
        "ordered4", "4x4 ordered dithering",
        "ordered8", "8x8 ordered dithering",
        "random", "random dithering",
        "fstein", "Floyd-Steinberg dithering",
        NULL, NULL
    };

    return list;
}