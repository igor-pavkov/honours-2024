int caca_set_dither_algorithm(caca_dither_t *d, char const *str)
{
    if(!strcasecmp(str, "none"))
    {
        d->algo_name = "none";
        d->init_dither = init_no_dither;
        d->get_dither = get_no_dither;
        d->increment_dither = increment_no_dither;
    }
    else if(!strcasecmp(str, "ordered2"))
    {
        d->algo_name = "ordered2";
        d->init_dither = init_ordered2_dither;
        d->get_dither = get_ordered2_dither;
        d->increment_dither = increment_ordered2_dither;
    }
    else if(!strcasecmp(str, "ordered4"))
    {
        d->algo_name = "ordered4";
        d->init_dither = init_ordered4_dither;
        d->get_dither = get_ordered4_dither;
        d->increment_dither = increment_ordered4_dither;
    }
    else if(!strcasecmp(str, "ordered8"))
    {
        d->algo_name = "ordered8";
        d->init_dither = init_ordered8_dither;
        d->get_dither = get_ordered8_dither;
        d->increment_dither = increment_ordered8_dither;
    }
    else if(!strcasecmp(str, "random"))
    {
        d->algo_name = "random";
        d->init_dither = init_random_dither;
        d->get_dither = get_random_dither;
        d->increment_dither = increment_random_dither;
    }
    else if(!strcasecmp(str, "fstein") || !strcasecmp(str, "default"))
    {
        d->algo_name = "fstein";
        d->init_dither = init_fstein_dither;
        d->get_dither = get_fstein_dither;
        d->increment_dither = increment_fstein_dither;
    }
    else
    {
        seterrno(EINVAL);
        return -1;
    }

    return 0;
}