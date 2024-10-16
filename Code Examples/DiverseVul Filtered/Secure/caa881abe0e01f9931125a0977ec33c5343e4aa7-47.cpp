static int pxa2xx_i2c_initfn(SysBusDevice *sbd)
{
    DeviceState *dev = DEVICE(sbd);
    PXA2xxI2CState *s = PXA2XX_I2C(dev);

    s->bus = i2c_init_bus(dev, "i2c");

    memory_region_init_io(&s->iomem, OBJECT(s), &pxa2xx_i2c_ops, s,
                          "pxa2xx-i2c", s->region_size);
    sysbus_init_mmio(sbd, &s->iomem);
    sysbus_init_irq(sbd, &s->irq);

    return 0;
}