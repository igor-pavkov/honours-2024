caca_dither_t *caca_create_dither(int bpp, int w, int h, int pitch,
                                    uint32_t rmask, uint32_t gmask,
                                    uint32_t bmask, uint32_t amask)
{
    caca_dither_t *d;
    int i;

    /* Minor sanity test */
    if(w < 0 || h < 0 || pitch < 0 || bpp > 32 || bpp < 8)
    {
        seterrno(EINVAL);
        return NULL;
    }

    d = malloc(sizeof(caca_dither_t));
    if(!d)
    {
        seterrno(ENOMEM);
        return NULL;
    }

    if(!lookup_initialised)
    {
        /* XXX: because we do not wish to be thread-safe, there is a slight
         * chance that the following code will be executed twice. It is
         * totally harmless. */
        init_lookup();
        lookup_initialised = 1;
    }

    d->bpp = bpp;
    d->has_palette = 0;
    d->has_alpha = amask ? 1 : 0;

    d->w = w;
    d->h = h;
    d->pitch = pitch;

    d->rmask = rmask;
    d->gmask = gmask;
    d->bmask = bmask;
    d->amask = amask;

    /* Load bitmasks */
    if(rmask || gmask || bmask || amask)
    {
        mask2shift(rmask, &d->rright, &d->rleft);
        mask2shift(gmask, &d->gright, &d->gleft);
        mask2shift(bmask, &d->bright, &d->bleft);
        mask2shift(amask, &d->aright, &d->aleft);
    }

    /* In 8 bpp mode, default to a grayscale palette */
    if(bpp == 8)
    {
        d->has_palette = 1;
        d->has_alpha = 0;
        for(i = 0; i < 256; i++)
        {
            d->red[i] = i * 0xfff / 256;
            d->green[i] = i * 0xfff / 256;
            d->blue[i] = i * 0xfff / 256;
        }
    }

    /* Default gamma value */
    d->gamma = 1.0;
    for(i = 0; i < 4096; i++)
        d->gammatab[i] = i;

    /* Default colour properties */
    d->brightness = 1.0;
    d->contrast = 1.0;

    /* Default features */
    d->antialias_name = "prefilter";
    d->antialias = 1;

    d->color_name = "full16";
    d->color = COLOR_MODE_FULL16;

    d->glyph_name = "ascii";
    d->glyphs = ascii_glyphs;
    d->glyph_count = sizeof(ascii_glyphs) / sizeof(*ascii_glyphs);

    d->algo_name = "fstein";
    d->init_dither = init_fstein_dither;
    d->get_dither = get_fstein_dither;
    d->increment_dither = increment_fstein_dither;

    d->invert = 0;

    return d;
}