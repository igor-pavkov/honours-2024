static void get_adma_description(SDHCIState *s, ADMADescr *dscr)
{
    uint32_t adma1 = 0;
    uint64_t adma2 = 0;
    hwaddr entry_addr = (hwaddr)s->admasysaddr;
    switch (SDHC_DMA_TYPE(s->hostctl1)) {
    case SDHC_CTRL_ADMA2_32:
        dma_memory_read(s->dma_as, entry_addr, &adma2, sizeof(adma2));
        adma2 = le64_to_cpu(adma2);
        /* The spec does not specify endianness of descriptor table.
         * We currently assume that it is LE.
         */
        dscr->addr = (hwaddr)extract64(adma2, 32, 32) & ~0x3ull;
        dscr->length = (uint16_t)extract64(adma2, 16, 16);
        dscr->attr = (uint8_t)extract64(adma2, 0, 7);
        dscr->incr = 8;
        break;
    case SDHC_CTRL_ADMA1_32:
        dma_memory_read(s->dma_as, entry_addr, &adma1, sizeof(adma1));
        adma1 = le32_to_cpu(adma1);
        dscr->addr = (hwaddr)(adma1 & 0xFFFFF000);
        dscr->attr = (uint8_t)extract32(adma1, 0, 7);
        dscr->incr = 4;
        if ((dscr->attr & SDHC_ADMA_ATTR_ACT_MASK) == SDHC_ADMA_ATTR_SET_LEN) {
            dscr->length = (uint16_t)extract32(adma1, 12, 16);
        } else {
            dscr->length = 4 * KiB;
        }
        break;
    case SDHC_CTRL_ADMA2_64:
        dma_memory_read(s->dma_as, entry_addr, &dscr->attr, 1);
        dma_memory_read(s->dma_as, entry_addr + 2, &dscr->length, 2);
        dscr->length = le16_to_cpu(dscr->length);
        dma_memory_read(s->dma_as, entry_addr + 4, &dscr->addr, 8);
        dscr->addr = le64_to_cpu(dscr->addr);
        dscr->attr &= (uint8_t) ~0xC0;
        dscr->incr = 12;
        break;
    }
}