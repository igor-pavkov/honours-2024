static void intel_hda_set_state_sts(IntelHDAState *d, const IntelHDAReg *reg, uint32_t old)
{
    intel_hda_update_irq(d);
}