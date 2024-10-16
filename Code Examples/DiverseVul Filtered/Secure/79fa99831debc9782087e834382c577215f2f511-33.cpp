static void intel_hda_set_rirb_wp(IntelHDAState *d, const IntelHDAReg *reg, uint32_t old)
{
    if (d->rirb_wp & ICH6_RIRBWP_RST) {
        d->rirb_wp = 0;
    }
}