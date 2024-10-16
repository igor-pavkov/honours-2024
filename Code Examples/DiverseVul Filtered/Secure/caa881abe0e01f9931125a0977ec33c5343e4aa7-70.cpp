static uint64_t pxa2xx_cppmnc_read(CPUARMState *env, const ARMCPRegInfo *ri)
{
    PXA2xxState *s = (PXA2xxState *)ri->opaque;
    return s->pmnc;
}