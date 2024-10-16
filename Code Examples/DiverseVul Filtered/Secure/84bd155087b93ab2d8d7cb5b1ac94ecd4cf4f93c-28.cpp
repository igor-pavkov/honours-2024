int caca_set_dither_charset(caca_dither_t *d, char const *str)
{
    if(!strcasecmp(str, "shades"))
    {
        d->glyph_name = "shades";
        d->glyphs = shades_glyphs;
        d->glyph_count = sizeof(shades_glyphs) / sizeof(*shades_glyphs);
    }
    else if(!strcasecmp(str, "blocks"))
    {
        d->glyph_name = "blocks";
        d->glyphs = blocks_glyphs;
        d->glyph_count = sizeof(blocks_glyphs) / sizeof(*blocks_glyphs);
    }
    else if(!strcasecmp(str, "ascii") || !strcasecmp(str, "default"))
    {
        d->glyph_name = "ascii";
        d->glyphs = ascii_glyphs;
        d->glyph_count = sizeof(ascii_glyphs) / sizeof(*ascii_glyphs);
    }
    else
    {
        seterrno(EINVAL);
        return -1;
    }

    return 0;
}