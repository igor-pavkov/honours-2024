static inline void pxa2xx_rtc_int_update(PXA2xxRTCState *s)
{
    qemu_set_irq(s->rtc_irq, !!(s->rtsr & 0x2553));
}