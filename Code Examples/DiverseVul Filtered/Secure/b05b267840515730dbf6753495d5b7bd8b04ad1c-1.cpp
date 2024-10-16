static int i2c_ddc_event(I2CSlave *i2c, enum i2c_event event)
{
    I2CDDCState *s = I2CDDC(i2c);

    if (event == I2C_START_SEND) {
        s->firstbyte = true;
    }

    return 0;
}