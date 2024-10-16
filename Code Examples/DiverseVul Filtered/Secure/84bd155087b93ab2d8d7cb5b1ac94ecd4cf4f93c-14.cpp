char const * const * caca_get_dither_charset_list(caca_dither_t const *d)
{
    static char const * const list[] =
    {
        "ascii", "plain ASCII",
        "shades", "CP437 shades",
        "blocks", "Unicode blocks",
        NULL, NULL
    };

    return list;
}