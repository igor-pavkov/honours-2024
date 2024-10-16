static void sdhci_sdma_transfer_multi_blocks(SDHCIState *s)
{
    bool page_aligned = false;
    unsigned int begin;
    const uint16_t block_size = s->blksize & BLOCK_SIZE_MASK;
    uint32_t boundary_chk = 1 << (((s->blksize & ~BLOCK_SIZE_MASK) >> 12) + 12);
    uint32_t boundary_count = boundary_chk - (s->sdmasysad % boundary_chk);

    if (!(s->trnmod & SDHC_TRNS_BLK_CNT_EN) || !s->blkcnt) {
        qemu_log_mask(LOG_UNIMP, "infinite transfer is not supported\n");
        return;
    }

    /* XXX: Some sd/mmc drivers (for example, u-boot-slp) do not account for
     * possible stop at page boundary if initial address is not page aligned,
     * allow them to work properly */
    if ((s->sdmasysad % boundary_chk) == 0) {
        page_aligned = true;
    }

    s->prnsts |= SDHC_DATA_INHIBIT | SDHC_DAT_LINE_ACTIVE;
    if (s->trnmod & SDHC_TRNS_READ) {
        s->prnsts |= SDHC_DOING_READ;
        while (s->blkcnt) {
            if (s->data_count == 0) {
                sdbus_read_data(&s->sdbus, s->fifo_buffer, block_size);
            }
            begin = s->data_count;
            if (((boundary_count + begin) < block_size) && page_aligned) {
                s->data_count = boundary_count + begin;
                boundary_count = 0;
             } else {
                s->data_count = block_size;
                boundary_count -= block_size - begin;
                if (s->trnmod & SDHC_TRNS_BLK_CNT_EN) {
                    s->blkcnt--;
                }
            }
            dma_memory_write(s->dma_as, s->sdmasysad,
                             &s->fifo_buffer[begin], s->data_count - begin);
            s->sdmasysad += s->data_count - begin;
            if (s->data_count == block_size) {
                s->data_count = 0;
            }
            if (page_aligned && boundary_count == 0) {
                break;
            }
        }
    } else {
        s->prnsts |= SDHC_DOING_WRITE;
        while (s->blkcnt) {
            begin = s->data_count;
            if (((boundary_count + begin) < block_size) && page_aligned) {
                s->data_count = boundary_count + begin;
                boundary_count = 0;
             } else {
                s->data_count = block_size;
                boundary_count -= block_size - begin;
            }
            dma_memory_read(s->dma_as, s->sdmasysad,
                            &s->fifo_buffer[begin], s->data_count - begin);
            s->sdmasysad += s->data_count - begin;
            if (s->data_count == block_size) {
                sdbus_write_data(&s->sdbus, s->fifo_buffer, block_size);
                s->data_count = 0;
                if (s->trnmod & SDHC_TRNS_BLK_CNT_EN) {
                    s->blkcnt--;
                }
            }
            if (page_aligned && boundary_count == 0) {
                break;
            }
        }
    }

    if (s->blkcnt == 0) {
        sdhci_end_transfer(s);
    } else {
        if (s->norintstsen & SDHC_NISEN_DMA) {
            s->norintsts |= SDHC_NIS_DMA;
        }
        sdhci_update_irq(s);
    }
}