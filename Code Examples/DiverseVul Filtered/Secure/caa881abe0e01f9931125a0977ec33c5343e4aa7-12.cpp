static inline void pxa2xx_rtc_pi_tick(void *opaque)
{
    PXA2xxRTCState *s = (PXA2xxRTCState *) opaque;
    s->rtsr |= (1 << 13);
    pxa2xx_rtc_piupdate(s);
    s->last_rtcpicr = 0;
    pxa2xx_rtc_alarm_update(s, s->rtsr);
    pxa2xx_rtc_int_update(s);
}