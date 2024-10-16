static inline void pxa2xx_rtc_alarm_update(PXA2xxRTCState *s,
                uint32_t rtsr)
{
    if ((rtsr & (1 << 2)) && !(rtsr & (1 << 0)))
        timer_mod(s->rtc_hz, s->last_hz +
                (((s->rtar - s->last_rcnr) * 1000 *
                  ((s->rttr & 0xffff) + 1)) >> 15));
    else
        timer_del(s->rtc_hz);

    if ((rtsr & (1 << 5)) && !(rtsr & (1 << 4)))
        timer_mod(s->rtc_rdal1, s->last_hz +
                (((s->rdar1 - s->last_rdcr) * 1000 *
                  ((s->rttr & 0xffff) + 1)) >> 15)); /* TODO: fixup */
    else
        timer_del(s->rtc_rdal1);

    if ((rtsr & (1 << 7)) && !(rtsr & (1 << 6)))
        timer_mod(s->rtc_rdal2, s->last_hz +
                (((s->rdar2 - s->last_rdcr) * 1000 *
                  ((s->rttr & 0xffff) + 1)) >> 15)); /* TODO: fixup */
    else
        timer_del(s->rtc_rdal2);

    if ((rtsr & 0x1200) == 0x1200 && !(rtsr & (1 << 8)))
        timer_mod(s->rtc_swal1, s->last_sw +
                        (s->swar1 - s->last_swcr) * 10); /* TODO: fixup */
    else
        timer_del(s->rtc_swal1);

    if ((rtsr & 0x1800) == 0x1800 && !(rtsr & (1 << 10)))
        timer_mod(s->rtc_swal2, s->last_sw +
                        (s->swar2 - s->last_swcr) * 10); /* TODO: fixup */
    else
        timer_del(s->rtc_swal2);

    if ((rtsr & 0xc000) == 0xc000 && !(rtsr & (1 << 13)))
        timer_mod(s->rtc_pi, s->last_pi +
                        (s->piar & 0xffff) - s->last_rtcpicr);
    else
        timer_del(s->rtc_pi);
}