void ide_dma_error(IDEState *s)
{
    ide_transfer_stop(s);
    s->error = ABRT_ERR;
    s->status = READY_STAT | ERR_STAT;
    ide_set_inactive(s);
    ide_set_irq(s->bus);
}