static void pxa2xx_i2c_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    SysBusDeviceClass *k = SYS_BUS_DEVICE_CLASS(klass);

    k->init = pxa2xx_i2c_initfn;
    dc->desc = "PXA2xx I2C Bus Controller";
    dc->vmsd = &vmstate_pxa2xx_i2c;
    dc->props = pxa2xx_i2c_properties;
}