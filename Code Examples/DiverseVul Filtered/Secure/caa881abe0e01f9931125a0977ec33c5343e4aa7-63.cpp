static int pxa2xx_ssp_init(SysBusDevice *sbd)
{
    DeviceState *dev = DEVICE(sbd);
    PXA2xxSSPState *s = PXA2XX_SSP(dev);

    sysbus_init_irq(sbd, &s->irq);

    memory_region_init_io(&s->iomem, OBJECT(s), &pxa2xx_ssp_ops, s,
                          "pxa2xx-ssp", 0x1000);
    sysbus_init_mmio(sbd, &s->iomem);
    register_savevm(dev, "pxa2xx_ssp", -1, 0,
                    pxa2xx_ssp_save, pxa2xx_ssp_load, s);

    s->bus = ssi_create_bus(dev, "ssi");
    return 0;
}