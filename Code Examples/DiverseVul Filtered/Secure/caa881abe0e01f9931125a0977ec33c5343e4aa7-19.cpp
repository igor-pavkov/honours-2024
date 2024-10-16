static void pxa2xx_fir_rx(void *opaque, const uint8_t *buf, int size)
{
    PXA2xxFIrState *s = (PXA2xxFIrState *) opaque;
    if (!(s->control[0] & (1 << 4)))			/* RXE */
        return;

    while (size --) {
        s->status[1] |= 1 << 4;				/* EOF */
        if (s->rx_len >= 64) {
            s->status[1] |= 1 << 6;			/* ROR */
            break;
        }

        if (s->control[2] & (1 << 3))			/* RXP */
            s->rx_fifo[(s->rx_start + s->rx_len ++) & 63] = *(buf ++);
        else
            s->rx_fifo[(s->rx_start + s->rx_len ++) & 63] = ~*(buf ++);
    }

    pxa2xx_fir_update(s);
}