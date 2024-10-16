void sdhci_common_realize(SDHCIState *s, Error **errp)
{
    ERRP_GUARD();

    sdhci_init_readonly_registers(s, errp);
    if (*errp) {
        return;
    }
    s->buf_maxsz = sdhci_get_fifolen(s);
    s->fifo_buffer = g_malloc0(s->buf_maxsz);

    memory_region_init_io(&s->iomem, OBJECT(s), s->io_ops, s, "sdhci",
                          SDHC_REGISTERS_MAP_SIZE);
}