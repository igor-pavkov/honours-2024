static void sdhci_check_capareg(SDHCIState *s, Error **errp)
{
    uint64_t msk = s->capareg;
    uint32_t val;
    bool y;

    switch (s->sd_spec_version) {
    case 4:
        val = FIELD_EX64(s->capareg, SDHC_CAPAB, BUS64BIT_V4);
        trace_sdhci_capareg("64-bit system bus (v4)", val);
        msk = FIELD_DP64(msk, SDHC_CAPAB, BUS64BIT_V4, 0);

        val = FIELD_EX64(s->capareg, SDHC_CAPAB, UHS_II);
        trace_sdhci_capareg("UHS-II", val);
        msk = FIELD_DP64(msk, SDHC_CAPAB, UHS_II, 0);

        val = FIELD_EX64(s->capareg, SDHC_CAPAB, ADMA3);
        trace_sdhci_capareg("ADMA3", val);
        msk = FIELD_DP64(msk, SDHC_CAPAB, ADMA3, 0);

    /* fallthrough */
    case 3:
        val = FIELD_EX64(s->capareg, SDHC_CAPAB, ASYNC_INT);
        trace_sdhci_capareg("async interrupt", val);
        msk = FIELD_DP64(msk, SDHC_CAPAB, ASYNC_INT, 0);

        val = FIELD_EX64(s->capareg, SDHC_CAPAB, SLOT_TYPE);
        if (val) {
            error_setg(errp, "slot-type not supported");
            return;
        }
        trace_sdhci_capareg("slot type", val);
        msk = FIELD_DP64(msk, SDHC_CAPAB, SLOT_TYPE, 0);

        if (val != 2) {
            val = FIELD_EX64(s->capareg, SDHC_CAPAB, EMBEDDED_8BIT);
            trace_sdhci_capareg("8-bit bus", val);
        }
        msk = FIELD_DP64(msk, SDHC_CAPAB, EMBEDDED_8BIT, 0);

        val = FIELD_EX64(s->capareg, SDHC_CAPAB, BUS_SPEED);
        trace_sdhci_capareg("bus speed mask", val);
        msk = FIELD_DP64(msk, SDHC_CAPAB, BUS_SPEED, 0);

        val = FIELD_EX64(s->capareg, SDHC_CAPAB, DRIVER_STRENGTH);
        trace_sdhci_capareg("driver strength mask", val);
        msk = FIELD_DP64(msk, SDHC_CAPAB, DRIVER_STRENGTH, 0);

        val = FIELD_EX64(s->capareg, SDHC_CAPAB, TIMER_RETUNING);
        trace_sdhci_capareg("timer re-tuning", val);
        msk = FIELD_DP64(msk, SDHC_CAPAB, TIMER_RETUNING, 0);

        val = FIELD_EX64(s->capareg, SDHC_CAPAB, SDR50_TUNING);
        trace_sdhci_capareg("use SDR50 tuning", val);
        msk = FIELD_DP64(msk, SDHC_CAPAB, SDR50_TUNING, 0);

        val = FIELD_EX64(s->capareg, SDHC_CAPAB, RETUNING_MODE);
        trace_sdhci_capareg("re-tuning mode", val);
        msk = FIELD_DP64(msk, SDHC_CAPAB, RETUNING_MODE, 0);

        val = FIELD_EX64(s->capareg, SDHC_CAPAB, CLOCK_MULT);
        trace_sdhci_capareg("clock multiplier", val);
        msk = FIELD_DP64(msk, SDHC_CAPAB, CLOCK_MULT, 0);

    /* fallthrough */
    case 2: /* default version */
        val = FIELD_EX64(s->capareg, SDHC_CAPAB, ADMA2);
        trace_sdhci_capareg("ADMA2", val);
        msk = FIELD_DP64(msk, SDHC_CAPAB, ADMA2, 0);

        val = FIELD_EX64(s->capareg, SDHC_CAPAB, ADMA1);
        trace_sdhci_capareg("ADMA1", val);
        msk = FIELD_DP64(msk, SDHC_CAPAB, ADMA1, 0);

        val = FIELD_EX64(s->capareg, SDHC_CAPAB, BUS64BIT);
        trace_sdhci_capareg("64-bit system bus (v3)", val);
        msk = FIELD_DP64(msk, SDHC_CAPAB, BUS64BIT, 0);

    /* fallthrough */
    case 1:
        y = FIELD_EX64(s->capareg, SDHC_CAPAB, TOUNIT);
        msk = FIELD_DP64(msk, SDHC_CAPAB, TOUNIT, 0);

        val = FIELD_EX64(s->capareg, SDHC_CAPAB, TOCLKFREQ);
        trace_sdhci_capareg(y ? "timeout (MHz)" : "Timeout (KHz)", val);
        if (sdhci_check_capab_freq_range(s, "timeout", val, errp)) {
            return;
        }
        msk = FIELD_DP64(msk, SDHC_CAPAB, TOCLKFREQ, 0);

        val = FIELD_EX64(s->capareg, SDHC_CAPAB, BASECLKFREQ);
        trace_sdhci_capareg(y ? "base (MHz)" : "Base (KHz)", val);
        if (sdhci_check_capab_freq_range(s, "base", val, errp)) {
            return;
        }
        msk = FIELD_DP64(msk, SDHC_CAPAB, BASECLKFREQ, 0);

        val = FIELD_EX64(s->capareg, SDHC_CAPAB, MAXBLOCKLENGTH);
        if (val >= 3) {
            error_setg(errp, "block size can be 512, 1024 or 2048 only");
            return;
        }
        trace_sdhci_capareg("max block length", sdhci_get_fifolen(s));
        msk = FIELD_DP64(msk, SDHC_CAPAB, MAXBLOCKLENGTH, 0);

        val = FIELD_EX64(s->capareg, SDHC_CAPAB, HIGHSPEED);
        trace_sdhci_capareg("high speed", val);
        msk = FIELD_DP64(msk, SDHC_CAPAB, HIGHSPEED, 0);

        val = FIELD_EX64(s->capareg, SDHC_CAPAB, SDMA);
        trace_sdhci_capareg("SDMA", val);
        msk = FIELD_DP64(msk, SDHC_CAPAB, SDMA, 0);

        val = FIELD_EX64(s->capareg, SDHC_CAPAB, SUSPRESUME);
        trace_sdhci_capareg("suspend/resume", val);
        msk = FIELD_DP64(msk, SDHC_CAPAB, SUSPRESUME, 0);

        val = FIELD_EX64(s->capareg, SDHC_CAPAB, V33);
        trace_sdhci_capareg("3.3v", val);
        msk = FIELD_DP64(msk, SDHC_CAPAB, V33, 0);

        val = FIELD_EX64(s->capareg, SDHC_CAPAB, V30);
        trace_sdhci_capareg("3.0v", val);
        msk = FIELD_DP64(msk, SDHC_CAPAB, V30, 0);

        val = FIELD_EX64(s->capareg, SDHC_CAPAB, V18);
        trace_sdhci_capareg("1.8v", val);
        msk = FIELD_DP64(msk, SDHC_CAPAB, V18, 0);
        break;

    default:
        error_setg(errp, "Unsupported spec version: %u", s->sd_spec_version);
    }
    if (msk) {
        qemu_log_mask(LOG_UNIMP,
                      "SDHCI: unknown CAPAB mask: 0x%016" PRIx64 "\n", msk);
    }
}