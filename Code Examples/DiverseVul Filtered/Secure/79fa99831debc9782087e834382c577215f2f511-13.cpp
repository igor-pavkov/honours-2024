static void intel_hda_set_corb_wp(IntelHDAState *d, const IntelHDAReg *reg, uint32_t old)
{
    intel_hda_corb_run(d);
}