static void pxa2xx_fir_reset(PXA2xxFIrState *s)
{
    s->control[0] = 0x00;
    s->control[1] = 0x00;
    s->control[2] = 0x00;
    s->status[0] = 0x00;
    s->status[1] = 0x00;
    s->enable = 0;
}