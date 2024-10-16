static bool sdhci_check_capab_freq_range(SDHCIState *s, const char *desc,
                                         uint8_t freq, Error **errp)
{
    if (s->sd_spec_version >= 3) {
        return false;
    }
    switch (freq) {
    case 0:
    case 10 ... 63:
        break;
    default:
        error_setg(errp, "SD %s clock frequency can have value"
                   "in range 0-63 only", desc);
        return true;
    }
    return false;
}