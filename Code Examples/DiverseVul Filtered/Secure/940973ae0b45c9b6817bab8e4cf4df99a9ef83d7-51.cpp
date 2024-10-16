static void ide_async_cmd_done(IDEState *s)
{
    if (s->bus->dma->ops->async_cmd_done) {
        s->bus->dma->ops->async_cmd_done(s->bus->dma);
    }
}