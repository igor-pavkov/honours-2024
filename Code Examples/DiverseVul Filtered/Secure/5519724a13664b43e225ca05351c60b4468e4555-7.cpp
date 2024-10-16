static void enet_update_irq(XgmacState *s)
{
    int stat = s->regs[DMA_STATUS] & s->regs[DMA_INTR_ENA];
    qemu_set_irq(s->sbd_irq, !!stat);
}