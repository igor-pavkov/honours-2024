static void sdhci_sdma_transfer_single_block(SDHCIState *s)
{
    uint32_t datacnt = s->blksize & BLOCK_SIZE_MASK;

    if (s->trnmod & SDHC_TRNS_READ) {
        sdbus_read_data(&s->sdbus, s->fifo_buffer, datacnt);
        dma_memory_write(s->dma_as, s->sdmasysad, s->fifo_buffer, datacnt);
    } else {
        dma_memory_read(s->dma_as, s->sdmasysad, s->fifo_buffer, datacnt);
        sdbus_write_data(&s->sdbus, s->fifo_buffer, datacnt);
    }
    s->blkcnt--;

    sdhci_end_transfer(s);
}