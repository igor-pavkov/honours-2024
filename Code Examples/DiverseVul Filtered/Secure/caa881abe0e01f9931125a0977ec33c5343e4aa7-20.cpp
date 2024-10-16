static int pxa2xx_rtc_init(SysBusDevice *dev)
{
    PXA2xxRTCState *s = PXA2XX_RTC(dev);
    struct tm tm;
    int wom;

    s->rttr = 0x7fff;
    s->rtsr = 0;

    qemu_get_timedate(&tm, 0);
    wom = ((tm.tm_mday - 1) / 7) + 1;

    s->last_rcnr = (uint32_t) mktimegm(&tm);
    s->last_rdcr = (wom << 20) | ((tm.tm_wday + 1) << 17) |
            (tm.tm_hour << 12) | (tm.tm_min << 6) | tm.tm_sec;
    s->last_rycr = ((tm.tm_year + 1900) << 9) |
            ((tm.tm_mon + 1) << 5) | tm.tm_mday;
    s->last_swcr = (tm.tm_hour << 19) |
            (tm.tm_min << 13) | (tm.tm_sec << 7);
    s->last_rtcpicr = 0;
    s->last_hz = s->last_sw = s->last_pi = qemu_clock_get_ms(rtc_clock);

    s->rtc_hz    = timer_new_ms(rtc_clock, pxa2xx_rtc_hz_tick,    s);
    s->rtc_rdal1 = timer_new_ms(rtc_clock, pxa2xx_rtc_rdal1_tick, s);
    s->rtc_rdal2 = timer_new_ms(rtc_clock, pxa2xx_rtc_rdal2_tick, s);
    s->rtc_swal1 = timer_new_ms(rtc_clock, pxa2xx_rtc_swal1_tick, s);
    s->rtc_swal2 = timer_new_ms(rtc_clock, pxa2xx_rtc_swal2_tick, s);
    s->rtc_pi    = timer_new_ms(rtc_clock, pxa2xx_rtc_pi_tick,    s);

    sysbus_init_irq(dev, &s->rtc_irq);

    memory_region_init_io(&s->iomem, OBJECT(s), &pxa2xx_rtc_ops, s,
                          "pxa2xx-rtc", 0x10000);
    sysbus_init_mmio(dev, &s->iomem);

    return 0;
}