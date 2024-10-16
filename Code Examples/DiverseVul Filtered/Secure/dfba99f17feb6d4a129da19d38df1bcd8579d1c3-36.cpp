static uint8_t sdhci_slotint(SDHCIState *s)
{
    return (s->norintsts & s->norintsigen) || (s->errintsts & s->errintsigen) ||
         ((s->norintsts & SDHC_NIS_INSERT) && (s->wakcon & SDHC_WKUP_ON_INS)) ||
         ((s->norintsts & SDHC_NIS_REMOVE) && (s->wakcon & SDHC_WKUP_ON_RMV));
}