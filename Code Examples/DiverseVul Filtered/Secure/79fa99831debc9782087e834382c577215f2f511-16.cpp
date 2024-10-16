static void intel_hda_set_g_ctl(IntelHDAState *d, const IntelHDAReg *reg, uint32_t old)
{
    if ((d->g_ctl & ICH6_GCTL_RESET) == 0) {
        intel_hda_reset(DEVICE(d));
    }
}