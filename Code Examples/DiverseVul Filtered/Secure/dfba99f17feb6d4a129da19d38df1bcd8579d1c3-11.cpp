static uint64_t sdhci_s3c_read(void *opaque, hwaddr offset, unsigned size)
{
    uint64_t ret;

    switch (offset) {
    case S3C_SDHCI_CONTROL2:
    case S3C_SDHCI_CONTROL3:
    case S3C_SDHCI_CONTROL4:
        /* ignore */
        ret = 0;
        break;
    default:
        ret = sdhci_read(opaque, offset, size);
        break;
    }

    return ret;
}