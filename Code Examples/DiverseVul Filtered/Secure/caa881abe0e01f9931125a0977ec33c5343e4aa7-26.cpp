static uint64_t pxa2xx_clkcfg_read(CPUARMState *env, const ARMCPRegInfo *ri)
{
    PXA2xxState *s = (PXA2xxState *)ri->opaque;
    return s->clkcfg;
}