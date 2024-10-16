static void pxa2xx_rtc_sysbus_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    SysBusDeviceClass *k = SYS_BUS_DEVICE_CLASS(klass);

    k->init = pxa2xx_rtc_init;
    dc->desc = "PXA2xx RTC Controller";
    dc->vmsd = &vmstate_pxa2xx_rtc_regs;
}