static void pxa2xx_rtc_swupdate(PXA2xxRTCState *s)
{
    int64_t rt = qemu_clock_get_ms(rtc_clock);
    if (s->rtsr & (1 << 12))
        s->last_swcr += (rt - s->last_sw) / 10;
    s->last_sw = rt;
}