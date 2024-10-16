static void ide_reset(IDEState *s)
{
#ifdef DEBUG_IDE
    printf("ide: reset\n");
#endif

    if (s->pio_aiocb) {
        bdrv_aio_cancel(s->pio_aiocb);
        s->pio_aiocb = NULL;
    }

    if (s->drive_kind == IDE_CFATA)
        s->mult_sectors = 0;
    else
        s->mult_sectors = MAX_MULT_SECTORS;
    /* ide regs */
    s->feature = 0;
    s->error = 0;
    s->nsector = 0;
    s->sector = 0;
    s->lcyl = 0;
    s->hcyl = 0;

    /* lba48 */
    s->hob_feature = 0;
    s->hob_sector = 0;
    s->hob_nsector = 0;
    s->hob_lcyl = 0;
    s->hob_hcyl = 0;

    s->select = 0xa0;
    s->status = READY_STAT | SEEK_STAT;

    s->lba48 = 0;

    /* ATAPI specific */
    s->sense_key = 0;
    s->asc = 0;
    s->cdrom_changed = 0;
    s->packet_transfer_size = 0;
    s->elementary_transfer_size = 0;
    s->io_buffer_index = 0;
    s->cd_sector_size = 0;
    s->atapi_dma = 0;
    s->tray_locked = 0;
    s->tray_open = 0;
    /* ATA DMA state */
    s->io_buffer_size = 0;
    s->req_nb_sectors = 0;

    ide_set_signature(s);
    /* init the transfer handler so that 0xffff is returned on data
       accesses */
    s->end_transfer_func = ide_dummy_transfer_stop;
    ide_dummy_transfer_stop(s);
    s->media_changed = 0;
}