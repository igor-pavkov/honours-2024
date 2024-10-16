int caca_set_dither_palette(caca_dither_t *d,
                             uint32_t red[], uint32_t green[],
                             uint32_t blue[], uint32_t alpha[])
{
    int i, has_alpha = 0;

    if(d->bpp != 8)
    {
        seterrno(EINVAL);
        return -1;
    }

    for(i = 0; i < 256; i++)
    {
        if((red[i] | green[i] | blue[i] | alpha[i]) >= 0x1000)
        {
            seterrno(EINVAL);
            return -1;
        }
    }

    for(i = 0; i < 256; i++)
    {
        d->red[i] = red[i];
        d->green[i] = green[i];
        d->blue[i] = blue[i];
        if(alpha[i])
        {
            d->alpha[i] = alpha[i];
            has_alpha = 1;
        }
    }

    d->has_alpha = has_alpha;

    return 0;
}