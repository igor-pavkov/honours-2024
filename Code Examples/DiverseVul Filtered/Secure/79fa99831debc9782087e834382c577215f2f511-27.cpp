static void intel_hda_get_wall_clk(IntelHDAState *d, const IntelHDAReg *reg)
{
    int64_t ns;

    ns = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) - d->wall_base_ns;
    d->wall_clk = (uint32_t)(ns * 24 / 1000);  /* 24 MHz */
}