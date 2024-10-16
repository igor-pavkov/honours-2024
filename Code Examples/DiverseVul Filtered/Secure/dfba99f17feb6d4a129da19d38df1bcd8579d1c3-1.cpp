static inline unsigned int sdhci_get_fifolen(SDHCIState *s)
{
    return 1 << (9 + FIELD_EX32(s->capareg, SDHC_CAPAB, MAXBLOCKLENGTH));
}