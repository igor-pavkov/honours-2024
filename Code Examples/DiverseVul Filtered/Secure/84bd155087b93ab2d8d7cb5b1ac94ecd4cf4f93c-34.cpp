int caca_set_dither_antialias(caca_dither_t *d, char const *str)
{
    if(!strcasecmp(str, "none"))
    {
        d->antialias_name = "none";
        d->antialias = 0;
    }
    else if(!strcasecmp(str, "prefilter") || !strcasecmp(str, "default"))
    {
        d->antialias_name = "prefilter";
        d->antialias = 1;
    }
    else
    {
        seterrno(EINVAL);
        return -1;
    }

    return 0;
}