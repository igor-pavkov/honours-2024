static void i2c_ddc_reset(DeviceState *ds)
{
    I2CDDCState *s = I2CDDC(ds);

    s->firstbyte = false;
    s->reg = 0;
}