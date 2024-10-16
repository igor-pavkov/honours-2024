void ide_atapi_dma_restart(IDEState *s)
{
    /*
     * At this point we can just re-evaluate the packet command and start over.
     * The presence of ->dma_cb callback in the pre_save ensures that the packet
     * command has been completely sent and we can safely restart command.
     */
    s->unit = s->bus->retry_unit;
    s->bus->dma->ops->restart_dma(s->bus->dma);
    ide_atapi_cmd(s);
}