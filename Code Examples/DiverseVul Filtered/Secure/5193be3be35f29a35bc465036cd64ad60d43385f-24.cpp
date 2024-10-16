uint32_t tsc210x_txrx(void *opaque, uint32_t value, int len)
{
    TSC210xState *s = opaque;
    uint32_t ret = 0;

    if (len != 16)
        hw_error("%s: FIXME: bad SPI word width %i\n", __FUNCTION__, len);

    /* TODO: sequential reads etc - how do we make sure the host doesn't
     * unintentionally read out a conversion result from a register while
     * transmitting the command word of the next command?  */
    if (!value || (s->state && s->command))
        ret = tsc210x_read(s);
    if (value || (s->state && !s->command))
        tsc210x_write(s, value);

    return ret;
}