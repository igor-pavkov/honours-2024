static void sdhci_raise_insertion_irq(void *opaque)
{
    SDHCIState *s = (SDHCIState *)opaque;

    if (s->norintsts & SDHC_NIS_REMOVE) {
        timer_mod(s->insert_timer,
                       qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) + SDHC_INSERTION_DELAY);
    } else {
        s->prnsts = 0x1ff0000;
        if (s->norintstsen & SDHC_NISEN_INSERT) {
            s->norintsts |= SDHC_NIS_INSERT;
        }
        sdhci_update_irq(s);
    }
}