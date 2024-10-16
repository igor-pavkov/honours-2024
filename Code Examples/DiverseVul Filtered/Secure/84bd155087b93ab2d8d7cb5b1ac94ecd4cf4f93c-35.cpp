int caca_free_dither(caca_dither_t *d)
{
    if(!d)
        return 0;

    free(d);

    return 0;
}