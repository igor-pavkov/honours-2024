static int i2c_ddc_tx(I2CSlave *i2c, uint8_t data)
{
    I2CDDCState *s = I2CDDC(i2c);
    if (s->firstbyte) {
        s->reg = data;
        s->firstbyte = false;
        DPRINTF("[EDID] Written new pointer: %u\n", data);
        return 0;
    }

    /* Ignore all writes */
    s->reg++;
    return 0;
}