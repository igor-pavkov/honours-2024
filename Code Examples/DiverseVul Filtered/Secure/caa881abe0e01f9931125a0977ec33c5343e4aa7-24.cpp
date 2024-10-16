PXA2xxI2CState *pxa2xx_i2c_init(hwaddr base,
                qemu_irq irq, uint32_t region_size)
{
    DeviceState *dev;
    SysBusDevice *i2c_dev;
    PXA2xxI2CState *s;
    I2CBus *i2cbus;

    dev = qdev_create(NULL, TYPE_PXA2XX_I2C);
    qdev_prop_set_uint32(dev, "size", region_size + 1);
    qdev_prop_set_uint32(dev, "offset", base & region_size);
    qdev_init_nofail(dev);

    i2c_dev = SYS_BUS_DEVICE(dev);
    sysbus_mmio_map(i2c_dev, 0, base & ~region_size);
    sysbus_connect_irq(i2c_dev, 0, irq);

    s = PXA2XX_I2C(i2c_dev);
    /* FIXME: Should the slave device really be on a separate bus?  */
    i2cbus = i2c_init_bus(dev, "dummy");
    dev = i2c_create_slave(i2cbus, TYPE_PXA2XX_I2C_SLAVE, 0);
    s->slave = PXA2XX_I2C_SLAVE(dev);
    s->slave->host = s;

    return s;
}