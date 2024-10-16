static bool cmd_cfa_translate_sector(IDEState *s, uint8_t cmd)
{
    s->status = READY_STAT | SEEK_STAT;

    memset(s->io_buffer, 0, 0x200);
    s->io_buffer[0x00] = s->hcyl;                   /* Cyl MSB */
    s->io_buffer[0x01] = s->lcyl;                   /* Cyl LSB */
    s->io_buffer[0x02] = s->select;                 /* Head */
    s->io_buffer[0x03] = s->sector;                 /* Sector */
    s->io_buffer[0x04] = ide_get_sector(s) >> 16;   /* LBA MSB */
    s->io_buffer[0x05] = ide_get_sector(s) >> 8;    /* LBA */
    s->io_buffer[0x06] = ide_get_sector(s) >> 0;    /* LBA LSB */
    s->io_buffer[0x13] = 0x00;                      /* Erase flag */
    s->io_buffer[0x18] = 0x00;                      /* Hot count */
    s->io_buffer[0x19] = 0x00;                      /* Hot count */
    s->io_buffer[0x1a] = 0x01;                      /* Hot count */

    ide_transfer_start(s, s->io_buffer, 0x200, ide_transfer_stop);
    ide_set_irq(s->bus);

    return false;
}