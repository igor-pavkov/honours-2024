static void ide_atapi_cmd_read_dma_cb(void *opaque, int ret)
{
    IDEState *s = opaque;
    int data_offset, n;

    if (ret < 0) {
        if (ide_handle_rw_error(s, -ret, ide_dma_cmd_to_retry(s->dma_cmd))) {
            if (s->bus->error_status) {
                s->bus->dma->aiocb = NULL;
                return;
            }
            goto eot;
        }
    }

    if (s->io_buffer_size > 0) {
        /*
         * For a cdrom read sector command (s->lba != -1),
         * adjust the lba for the next s->io_buffer_size chunk
         * and dma the current chunk.
         * For a command != read (s->lba == -1), just transfer
         * the reply data.
         */
        if (s->lba != -1) {
            if (s->cd_sector_size == 2352) {
                n = 1;
                cd_data_to_raw(s->io_buffer, s->lba);
            } else {
                n = s->io_buffer_size >> 11;
            }
            s->lba += n;
        }
        s->packet_transfer_size -= s->io_buffer_size;
        if (s->bus->dma->ops->rw_buf(s->bus->dma, 1) == 0)
            goto eot;
    }

    if (s->packet_transfer_size <= 0) {
        s->status = READY_STAT | SEEK_STAT;
        s->nsector = (s->nsector & ~7) | ATAPI_INT_REASON_IO | ATAPI_INT_REASON_CD;
        ide_set_irq(s->bus);
        goto eot;
    }

    s->io_buffer_index = 0;
    if (s->cd_sector_size == 2352) {
        n = 1;
        s->io_buffer_size = s->cd_sector_size;
        data_offset = 16;
    } else {
        n = s->packet_transfer_size >> 11;
        if (n > (IDE_DMA_BUF_SECTORS / 4))
            n = (IDE_DMA_BUF_SECTORS / 4);
        s->io_buffer_size = n * 2048;
        data_offset = 0;
    }
    trace_ide_atapi_cmd_read_dma_cb_aio(s, s->lba, n);
    qemu_iovec_init_buf(&s->bus->dma->qiov, s->io_buffer + data_offset,
                        n * ATAPI_SECTOR_SIZE);

    s->bus->dma->aiocb = ide_buffered_readv(s, (int64_t)s->lba << 2,
                                            &s->bus->dma->qiov, n * 4,
                                            ide_atapi_cmd_read_dma_cb, s);
    return;

eot:
    if (ret < 0) {
        block_acct_failed(blk_get_stats(s->blk), &s->acct);
    } else {
        block_acct_done(blk_get_stats(s->blk), &s->acct);
    }
    ide_set_inactive(s, false);
}