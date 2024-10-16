static void intel_hda_set_rirb_sts(IntelHDAState *d, const IntelHDAReg *reg, uint32_t old)
{
    intel_hda_update_irq(d);

    if ((old & ICH6_RBSTS_IRQ) && !(d->rirb_sts & ICH6_RBSTS_IRQ)) {
        /* cleared ICH6_RBSTS_IRQ */
        d->rirb_count = 0;
        intel_hda_corb_run(d);
    }
}