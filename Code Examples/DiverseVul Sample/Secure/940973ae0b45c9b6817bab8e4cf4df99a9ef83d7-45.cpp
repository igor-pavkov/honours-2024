static void ide_cfata_metadata_inquiry(IDEState *s)
{
    uint16_t *p;
    uint32_t spd;

    p = (uint16_t *) s->io_buffer;
    memset(p, 0, 0x200);
    spd = ((s->mdata_size - 1) >> 9) + 1;

    put_le16(p + 0, 0x0001);			/* Data format revision */
    put_le16(p + 1, 0x0000);			/* Media property: silicon */
    put_le16(p + 2, s->media_changed);		/* Media status */
    put_le16(p + 3, s->mdata_size & 0xffff);	/* Capacity in bytes (low) */
    put_le16(p + 4, s->mdata_size >> 16);	/* Capacity in bytes (high) */
    put_le16(p + 5, spd & 0xffff);		/* Sectors per device (low) */
    put_le16(p + 6, spd >> 16);			/* Sectors per device (high) */
}