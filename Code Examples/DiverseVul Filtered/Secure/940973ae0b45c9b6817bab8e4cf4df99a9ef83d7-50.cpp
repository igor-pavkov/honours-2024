static void ide_sector_write_timer_cb(void *opaque)
{
    IDEState *s = opaque;
    ide_set_irq(s->bus);
}