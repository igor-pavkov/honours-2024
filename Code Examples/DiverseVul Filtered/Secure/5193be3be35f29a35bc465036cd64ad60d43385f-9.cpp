static void tsc210x_timer_tick(void *opaque)
{
    TSC210xState *s = opaque;

    /* Timer ticked -- a set of conversions has been finished.  */

    if (!s->busy)
        return;

    s->busy = 0;
    s->dav |= mode_regs[s->function];
    tsc210x_pin_update(s);
    qemu_irq_lower(s->davint);
}