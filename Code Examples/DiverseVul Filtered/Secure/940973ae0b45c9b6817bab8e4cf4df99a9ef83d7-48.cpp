void ide_transfer_start(IDEState *s, uint8_t *buf, int size,
                        EndTransferFunc *end_transfer_func)
{
    s->end_transfer_func = end_transfer_func;
    s->data_ptr = buf;
    s->data_end = buf + size;
    if (!(s->status & ERR_STAT)) {
        s->status |= DRQ_STAT;
    }
    s->bus->dma->ops->start_transfer(s->bus->dma);
}