static int eth_can_rx(XgmacState *s)
{
    /* RX enabled?  */
    return s->regs[DMA_CONTROL] & DMA_CONTROL_SR;
}