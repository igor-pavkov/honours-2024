static void sdhci_read_block_from_card(SDHCIState *s)
{
    const uint16_t blk_size = s->blksize & BLOCK_SIZE_MASK;

    if ((s->trnmod & SDHC_TRNS_MULTI) &&
            (s->trnmod & SDHC_TRNS_BLK_CNT_EN) && (s->blkcnt == 0)) {
        return;
    }

    if (!FIELD_EX32(s->hostctl2, SDHC_HOSTCTL2, EXECUTE_TUNING)) {
        /* Device is not in tuning */
        sdbus_read_data(&s->sdbus, s->fifo_buffer, blk_size);
    }

    if (FIELD_EX32(s->hostctl2, SDHC_HOSTCTL2, EXECUTE_TUNING)) {
        /* Device is in tuning */
        s->hostctl2 &= ~R_SDHC_HOSTCTL2_EXECUTE_TUNING_MASK;
        s->hostctl2 |= R_SDHC_HOSTCTL2_SAMPLING_CLKSEL_MASK;
        s->prnsts &= ~(SDHC_DAT_LINE_ACTIVE | SDHC_DOING_READ |
                       SDHC_DATA_INHIBIT);
        goto read_done;
    }

    /* New data now available for READ through Buffer Port Register */
    s->prnsts |= SDHC_DATA_AVAILABLE;
    if (s->norintstsen & SDHC_NISEN_RBUFRDY) {
        s->norintsts |= SDHC_NIS_RBUFRDY;
    }

    /* Clear DAT line active status if that was the last block */
    if ((s->trnmod & SDHC_TRNS_MULTI) == 0 ||
            ((s->trnmod & SDHC_TRNS_MULTI) && s->blkcnt == 1)) {
        s->prnsts &= ~SDHC_DAT_LINE_ACTIVE;
    }

    /* If stop at block gap request was set and it's not the last block of
     * data - generate Block Event interrupt */
    if (s->stopped_state == sdhc_gap_read && (s->trnmod & SDHC_TRNS_MULTI) &&
            s->blkcnt != 1)    {
        s->prnsts &= ~SDHC_DAT_LINE_ACTIVE;
        if (s->norintstsen & SDHC_EISEN_BLKGAP) {
            s->norintsts |= SDHC_EIS_BLKGAP;
        }
    }

read_done:
    sdhci_update_irq(s);
}