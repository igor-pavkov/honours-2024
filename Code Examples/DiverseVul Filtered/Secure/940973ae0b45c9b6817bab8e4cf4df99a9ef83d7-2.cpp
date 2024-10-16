static bool cmd_packet(IDEState *s, uint8_t cmd)
{
    /* overlapping commands not supported */
    if (s->feature & 0x02) {
        ide_abort_command(s);
        return true;
    }

    s->status = READY_STAT | SEEK_STAT;
    s->atapi_dma = s->feature & 1;
    s->nsector = 1;
    ide_transfer_start(s, s->io_buffer, ATAPI_PACKET_SIZE,
                       ide_atapi_cmd);
    return false;
}