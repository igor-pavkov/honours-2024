int caca_set_dither_gamma(caca_dither_t *d, float gamma)
{
    /* FIXME: we don't need 4096 calls to gammapow(), we could just compute
     * a few of them and do linear interpolation for the rest. This will
     * probably speed up things a lot. */
    int i;

    if(gamma < 0.0)
    {
        d->invert = 1;
        gamma = -gamma;
    }
    else if(gamma == 0.0)
    {
        seterrno(EINVAL);
        return -1;
    }

    d->gamma = gamma;

    for(i = 0; i < 4096; i++)
        d->gammatab[i] = 4096.0 * gammapow((float)i / 4096.0, 1.0 / gamma);

    return 0;
}