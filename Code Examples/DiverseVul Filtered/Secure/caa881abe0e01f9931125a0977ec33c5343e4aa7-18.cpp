static void pxa2xx_i2c_event(I2CSlave *i2c, enum i2c_event event)
{
    PXA2xxI2CSlaveState *slave = PXA2XX_I2C_SLAVE(i2c);
    PXA2xxI2CState *s = slave->host;

    switch (event) {
    case I2C_START_SEND:
        s->status |= (1 << 9);				/* set SAD */
        s->status &= ~(1 << 0);				/* clear RWM */
        break;
    case I2C_START_RECV:
        s->status |= (1 << 9);				/* set SAD */
        s->status |= 1 << 0;				/* set RWM */
        break;
    case I2C_FINISH:
        s->status |= (1 << 4);				/* set SSD */
        break;
    case I2C_NACK:
        s->status |= 1 << 1;				/* set ACKNAK */
        break;
    }
    pxa2xx_i2c_update(s);
}