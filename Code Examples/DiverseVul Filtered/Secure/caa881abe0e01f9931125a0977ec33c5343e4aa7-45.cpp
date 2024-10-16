static int pxa2xx_fir_is_empty(void *opaque)
{
    PXA2xxFIrState *s = (PXA2xxFIrState *) opaque;
    return (s->rx_len < 64);
}