void ide_set_inactive(IDEState *s)
{
    s->bus->dma->aiocb = NULL;
    s->bus->dma->ops->set_inactive(s->bus->dma);
    ide_async_cmd_done(s);
}