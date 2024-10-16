static void pxa2xx_i2s_data_req(void *opaque, int tx, int rx)
{
    PXA2xxI2SState *s = (PXA2xxI2SState *) opaque;
    uint32_t *sample;

    /* Signal FIFO errors */
    if (s->enable && s->tx_len)
        s->status |= 1 << 5;		/* TUR */
    if (s->enable && s->rx_len)
        s->status |= 1 << 6;		/* ROR */

    /* Should be tx - MIN(tx, s->fifo_len) but we don't really need to
     * handle the cases where it makes a difference.  */
    s->tx_len = tx - s->fifo_len;
    s->rx_len = rx;
    /* Note that is s->codec_out wasn't set, we wouldn't get called.  */
    if (s->enable)
        for (sample = s->fifo; s->fifo_len; s->fifo_len --, sample ++)
            s->codec_out(s->opaque, *sample);
    pxa2xx_i2s_update(s);
}