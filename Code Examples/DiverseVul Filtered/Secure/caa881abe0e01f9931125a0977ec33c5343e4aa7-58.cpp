static void pxa2xx_clkcfg_write(CPUARMState *env, const ARMCPRegInfo *ri,
                                uint64_t value)
{
    PXA2xxState *s = (PXA2xxState *)ri->opaque;
    s->clkcfg = value & 0xf;
    if (value & 2) {
        printf("%s: CPU frequency change attempt\n", __func__);
    }
}