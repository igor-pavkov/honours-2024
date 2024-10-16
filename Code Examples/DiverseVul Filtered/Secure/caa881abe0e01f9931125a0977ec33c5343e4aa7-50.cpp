static int pxa2xx_rtc_post_load(void *opaque, int version_id)
{
    PXA2xxRTCState *s = (PXA2xxRTCState *) opaque;

    pxa2xx_rtc_alarm_update(s, s->rtsr);

    return 0;
}