static inline void pxa2xx_i2s_update(PXA2xxI2SState *i2s)
{
    int rfs, tfs;
    rfs = SACR_RFTH(i2s->control[0]) < i2s->rx_len &&
            !SACR_DREC(i2s->control[1]);
    tfs = (i2s->tx_len || i2s->fifo_len < SACR_TFTH(i2s->control[0])) &&
            i2s->enable && !SACR_DPRL(i2s->control[1]);

    qemu_set_irq(i2s->rx_dma, rfs);
    qemu_set_irq(i2s->tx_dma, tfs);

    i2s->status &= 0xe0;
    if (i2s->fifo_len < 16 || !i2s->enable)
        i2s->status |= 1 << 0;			/* TNF */
    if (i2s->rx_len)
        i2s->status |= 1 << 1;			/* RNE */
    if (i2s->enable)
        i2s->status |= 1 << 2;			/* BSY */
    if (tfs)
        i2s->status |= 1 << 3;			/* TFS */
    if (rfs)
        i2s->status |= 1 << 4;			/* RFS */
    if (!(i2s->tx_len && i2s->enable))
        i2s->status |= i2s->fifo_len << 8;	/* TFL */
    i2s->status |= MAX(i2s->rx_len, 0xf) << 12;	/* RFL */

    qemu_set_irq(i2s->irq, i2s->status & i2s->mask);
}