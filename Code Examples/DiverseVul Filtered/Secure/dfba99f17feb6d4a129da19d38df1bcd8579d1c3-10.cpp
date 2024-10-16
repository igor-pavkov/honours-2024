static void sdhci_data_transfer(void *opaque)
{
    SDHCIState *s = (SDHCIState *)opaque;

    if (s->trnmod & SDHC_TRNS_DMA) {
        switch (SDHC_DMA_TYPE(s->hostctl1)) {
        case SDHC_CTRL_SDMA:
            if ((s->blkcnt == 1) || !(s->trnmod & SDHC_TRNS_MULTI)) {
                sdhci_sdma_transfer_single_block(s);
            } else {
                sdhci_sdma_transfer_multi_blocks(s);
            }

            break;
        case SDHC_CTRL_ADMA1_32:
            if (!(s->capareg & R_SDHC_CAPAB_ADMA1_MASK)) {
                trace_sdhci_error("ADMA1 not supported");
                break;
            }

            sdhci_do_adma(s);
            break;
        case SDHC_CTRL_ADMA2_32:
            if (!(s->capareg & R_SDHC_CAPAB_ADMA2_MASK)) {
                trace_sdhci_error("ADMA2 not supported");
                break;
            }

            sdhci_do_adma(s);
            break;
        case SDHC_CTRL_ADMA2_64:
            if (!(s->capareg & R_SDHC_CAPAB_ADMA2_MASK) ||
                    !(s->capareg & R_SDHC_CAPAB_BUS64BIT_MASK)) {
                trace_sdhci_error("64 bit ADMA not supported");
                break;
            }

            sdhci_do_adma(s);
            break;
        default:
            trace_sdhci_error("Unsupported DMA type");
            break;
        }
    } else {
        if ((s->trnmod & SDHC_TRNS_READ) && sdbus_data_ready(&s->sdbus)) {
            s->prnsts |= SDHC_DOING_READ | SDHC_DATA_INHIBIT |
                    SDHC_DAT_LINE_ACTIVE;
            sdhci_read_block_from_card(s);
        } else {
            s->prnsts |= SDHC_DOING_WRITE | SDHC_DAT_LINE_ACTIVE |
                    SDHC_SPACE_AVAILABLE | SDHC_DATA_INHIBIT;
            sdhci_write_block_to_card(s);
        }
    }
}