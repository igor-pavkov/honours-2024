static bool cmd_identify_packet(IDEState *s, uint8_t cmd)
{
    ide_atapi_identify(s);
    s->status = READY_STAT | SEEK_STAT;
    ide_transfer_start(s, s->io_buffer, 512, ide_transfer_stop);
    ide_set_irq(s->bus);
    return false;
}