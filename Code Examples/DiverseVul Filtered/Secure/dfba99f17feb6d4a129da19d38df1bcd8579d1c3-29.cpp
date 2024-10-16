static void sdhci_init_readonly_registers(SDHCIState *s, Error **errp)
{
    ERRP_GUARD();

    switch (s->sd_spec_version) {
    case 2 ... 3:
        break;
    default:
        error_setg(errp, "Only Spec v2/v3 are supported");
        return;
    }
    s->version = (SDHC_HCVER_VENDOR << 8) | (s->sd_spec_version - 1);

    sdhci_check_capareg(s, errp);
    if (*errp) {
        return;
    }
}