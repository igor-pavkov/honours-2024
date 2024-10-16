static uint64_t usdhc_read(void *opaque, hwaddr offset, unsigned size)
{
    SDHCIState *s = SYSBUS_SDHCI(opaque);
    uint32_t ret;
    uint16_t hostctl1;

    switch (offset) {
    default:
        return sdhci_read(opaque, offset, size);

    case SDHC_HOSTCTL:
        /*
         * For a detailed explanation on the following bit
         * manipulation code see comments in a similar part of
         * usdhc_write()
         */
        hostctl1 = SDHC_DMA_TYPE(s->hostctl1) << (8 - 3);

        if (s->hostctl1 & SDHC_CTRL_8BITBUS) {
            hostctl1 |= ESDHC_CTRL_8BITBUS;
        }

        if (s->hostctl1 & SDHC_CTRL_4BITBUS) {
            hostctl1 |= ESDHC_CTRL_4BITBUS;
        }

        ret  = hostctl1;
        ret |= (uint32_t)s->blkgap << 16;
        ret |= (uint32_t)s->wakcon << 24;

        break;

    case SDHC_PRNSTS:
        /* Add SDSTB (SD Clock Stable) bit to PRNSTS */
        ret = sdhci_read(opaque, offset, size) & ~ESDHC_PRNSTS_SDSTB;
        if (s->clkcon & SDHC_CLOCK_INT_STABLE) {
            ret |= ESDHC_PRNSTS_SDSTB;
        }
        break;

    case ESDHC_VENDOR_SPEC:
        ret = s->vendor_spec;
        break;
    case ESDHC_DLL_CTRL:
    case ESDHC_TUNE_CTRL_STATUS:
    case ESDHC_UNDOCUMENTED_REG27:
    case ESDHC_TUNING_CTRL:
    case ESDHC_MIX_CTRL:
    case ESDHC_WTMK_LVL:
        ret = 0;
        break;
    }

    return ret;
}