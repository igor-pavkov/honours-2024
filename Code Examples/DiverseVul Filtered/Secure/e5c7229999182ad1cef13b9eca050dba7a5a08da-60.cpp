static av_cold void avcodec_init(void)
{
    static int initialized = 0;

    if (initialized != 0)
        return;
    initialized = 1;

    if (CONFIG_DSPUTIL)
        ff_dsputil_static_init();
}