static void pxa2xx_rtc_piupdate(PXA2xxRTCState *s)
{
    int64_t rt = qemu_clock_get_ms(rtc_clock);
    if (s->rtsr & (1 << 15))
        s->last_swcr += rt - s->last_pi;
    s->last_pi = rt;
}