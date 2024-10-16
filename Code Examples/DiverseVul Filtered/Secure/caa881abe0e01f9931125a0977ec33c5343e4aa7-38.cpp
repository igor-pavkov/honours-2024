static inline void pxa2xx_fir_update(PXA2xxFIrState *s)
{
    static const int tresh[4] = { 8, 16, 32, 0 };
    int intr = 0;
    if ((s->control[0] & (1 << 4)) &&			/* RXE */
                    s->rx_len >= tresh[s->control[2] & 3])	/* TRIG */
        s->status[0] |= 1 << 4;				/* RFS */
    else
        s->status[0] &= ~(1 << 4);			/* RFS */
    if (s->control[0] & (1 << 3))			/* TXE */
        s->status[0] |= 1 << 3;				/* TFS */
    else
        s->status[0] &= ~(1 << 3);			/* TFS */
    if (s->rx_len)
        s->status[1] |= 1 << 2;				/* RNE */
    else
        s->status[1] &= ~(1 << 2);			/* RNE */
    if (s->control[0] & (1 << 4))			/* RXE */
        s->status[1] |= 1 << 0;				/* RSY */
    else
        s->status[1] &= ~(1 << 0);			/* RSY */

    intr |= (s->control[0] & (1 << 5)) &&		/* RIE */
            (s->status[0] & (1 << 4));			/* RFS */
    intr |= (s->control[0] & (1 << 6)) &&		/* TIE */
            (s->status[0] & (1 << 3));			/* TFS */
    intr |= (s->control[2] & (1 << 4)) &&		/* TRAIL */
            (s->status[0] & (1 << 6));			/* EOC */
    intr |= (s->control[0] & (1 << 2)) &&		/* TUS */
            (s->status[0] & (1 << 1));			/* TUR */
    intr |= s->status[0] & 0x25;			/* FRE, RAB, EIF */

    qemu_set_irq(s->rx_dma, (s->status[0] >> 4) & 1);
    qemu_set_irq(s->tx_dma, (s->status[0] >> 3) & 1);

    qemu_set_irq(s->irq, intr && s->enable);
}