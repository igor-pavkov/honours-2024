static void pxa2xx_cppmnc_write(CPUARMState *env, const ARMCPRegInfo *ri,
                                uint64_t value)
{
    PXA2xxState *s = (PXA2xxState *)ri->opaque;
    s->pmnc = value;
}