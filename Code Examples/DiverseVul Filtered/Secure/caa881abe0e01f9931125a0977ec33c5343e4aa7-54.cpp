static void pxa2xx_rtc_pre_save(void *opaque)
{
    PXA2xxRTCState *s = (PXA2xxRTCState *) opaque;

    pxa2xx_rtc_hzupdate(s);
    pxa2xx_rtc_piupdate(s);
    pxa2xx_rtc_swupdate(s);
}