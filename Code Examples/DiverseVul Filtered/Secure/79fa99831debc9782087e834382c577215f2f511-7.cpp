static void intel_hda_set_int_ctl(IntelHDAState *d, const IntelHDAReg *reg, uint32_t old)
{
    intel_hda_update_irq(d);
}