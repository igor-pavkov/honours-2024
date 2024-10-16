static uint64_t pxa2xx_cpccnt_read(CPUARMState *env, const ARMCPRegInfo *ri)
{
    PXA2xxState *s = (PXA2xxState *)ri->opaque;
    if (s->pmnc & 1) {
        return qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
    } else {
        return 0;
    }
}