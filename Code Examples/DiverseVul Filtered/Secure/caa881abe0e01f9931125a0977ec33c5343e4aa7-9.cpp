static void pxa2xx_setup_cp14(PXA2xxState *s)
{
    define_arm_cp_regs_with_opaque(s->cpu, pxa_cp_reginfo, s);
}