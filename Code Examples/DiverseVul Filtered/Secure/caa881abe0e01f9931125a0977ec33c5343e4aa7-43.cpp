static int pxa2xx_i2c_rx(I2CSlave *i2c)
{
    PXA2xxI2CSlaveState *slave = PXA2XX_I2C_SLAVE(i2c);
    PXA2xxI2CState *s = slave->host;

    if ((s->control & (1 << 14)) || !(s->control & (1 << 6))) {
        return 0;
    }

    if (s->status & (1 << 0)) {			/* RWM */
        s->status |= 1 << 6;			/* set ITE */
    }
    pxa2xx_i2c_update(s);

    return s->data;
}