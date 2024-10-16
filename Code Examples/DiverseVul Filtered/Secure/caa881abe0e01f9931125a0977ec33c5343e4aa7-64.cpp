static inline void pxa2xx_rtc_swal1_tick(void *opaque)
{
    PXA2xxRTCState *s = (PXA2xxRTCState *) opaque;
    s->rtsr |= (1 << 8);
    pxa2xx_rtc_alarm_update(s, s->rtsr);
    pxa2xx_rtc_int_update(s);
}