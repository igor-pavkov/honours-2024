void sdhci_common_unrealize(SDHCIState *s)
{
    /* This function is expected to be called only once for each class:
     * - SysBus:    via DeviceClass->unrealize(),
     * - PCI:       via PCIDeviceClass->exit().
     * However to avoid double-free and/or use-after-free we still nullify
     * this variable (better safe than sorry!). */
    g_free(s->fifo_buffer);
    s->fifo_buffer = NULL;
}