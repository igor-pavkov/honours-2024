static void sdhci_s3c_write(void *opaque, hwaddr offset, uint64_t val,
                            unsigned size)
{
    switch (offset) {
    case S3C_SDHCI_CONTROL2:
    case S3C_SDHCI_CONTROL3:
    case S3C_SDHCI_CONTROL4:
        /* ignore */
        break;
    default:
        sdhci_write(opaque, offset, val, size);
        break;
    }
}