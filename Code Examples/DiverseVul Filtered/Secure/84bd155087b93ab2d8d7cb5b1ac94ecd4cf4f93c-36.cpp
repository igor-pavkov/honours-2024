static void get_rgba_default(caca_dither_t const *d, uint8_t const *pixels,
                             int x, int y, unsigned int *rgba)
{
    uint32_t bits;

    pixels += (d->bpp / 8) * x + d->pitch * y;

    switch(d->bpp / 8)
    {
        case 4:
            bits = *(uint32_t const *)pixels;
            break;
        case 3:
        {
#if defined(HAVE_ENDIAN_H)
            if(__BYTE_ORDER == __BIG_ENDIAN)
#else
            /* This is compile-time optimised with at least -O1 or -Os */
            uint32_t const tmp = 0x12345678;
            if(*(uint8_t const *)&tmp == 0x12)
#endif
                bits = ((uint32_t)pixels[0] << 16) |
                       ((uint32_t)pixels[1] << 8) |
                       ((uint32_t)pixels[2]);
            else
                bits = ((uint32_t)pixels[2] << 16) |
                       ((uint32_t)pixels[1] << 8) |
                       ((uint32_t)pixels[0]);
            break;
        }
        case 2:
            bits = *(uint16_t const *)pixels;
            break;
        case 1:
        default:
            bits = pixels[0];
            break;
    }

    if(d->has_palette)
    {
        rgba[0] += d->gammatab[d->red[bits]];
        rgba[1] += d->gammatab[d->green[bits]];
        rgba[2] += d->gammatab[d->blue[bits]];
        rgba[3] += d->alpha[bits];
    }
    else
    {
        rgba[0] += d->gammatab[((bits & d->rmask) >> d->rright) << d->rleft];
        rgba[1] += d->gammatab[((bits & d->gmask) >> d->gright) << d->gleft];
        rgba[2] += d->gammatab[((bits & d->bmask) >> d->bright) << d->bleft];
        rgba[3] += ((bits & d->amask) >> d->aright) << d->aleft;
    }
}