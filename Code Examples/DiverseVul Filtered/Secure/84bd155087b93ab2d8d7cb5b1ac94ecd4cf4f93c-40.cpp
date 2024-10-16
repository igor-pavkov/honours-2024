int caca_set_dither_color(caca_dither_t *d, char const *str)
{
    if(!strcasecmp(str, "mono"))
    {
        d->color_name = "mono";
        d->color = COLOR_MODE_MONO;
    }
    else if(!strcasecmp(str, "gray"))
    {
        d->color_name = "gray";
        d->color = COLOR_MODE_GRAY;
    }
    else if(!strcasecmp(str, "8"))
    {
        d->color_name = "8";
        d->color = COLOR_MODE_8;
    }
    else if(!strcasecmp(str, "16"))
    {
        d->color_name = "16";
        d->color = COLOR_MODE_16;
    }
    else if(!strcasecmp(str, "fullgray"))
    {
        d->color_name = "fullgray";
        d->color = COLOR_MODE_FULLGRAY;
    }
    else if(!strcasecmp(str, "full8"))
    {
        d->color_name = "full8";
        d->color = COLOR_MODE_FULL8;
    }
    else if(!strcasecmp(str, "full16") || !strcasecmp(str, "default"))
    {
        d->color_name = "full16";
        d->color = COLOR_MODE_FULL16;
    }
    else
    {
        seterrno(EINVAL);
        return -1;
    }

    return 0;
}