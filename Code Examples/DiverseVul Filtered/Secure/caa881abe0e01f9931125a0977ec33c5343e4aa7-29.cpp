static void pxa2xx_reset(void *opaque, int line, int level)
{
    PXA2xxState *s = (PXA2xxState *) opaque;

    if (level && (s->pm_regs[PCFR >> 2] & 0x10)) {	/* GPR_EN */
        cpu_reset(CPU(s->cpu));
        /* TODO: reset peripherals */
    }
}