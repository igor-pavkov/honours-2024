static void intel_hda_set_ics(IntelHDAState *d, const IntelHDAReg *reg, uint32_t old)
{
    if (d->ics & ICH6_IRS_BUSY) {
        intel_hda_corb_run(d);
    }
}