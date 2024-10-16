static PXA2xxFIrState *pxa2xx_fir_init(MemoryRegion *sysmem,
                hwaddr base,
                qemu_irq irq, qemu_irq rx_dma, qemu_irq tx_dma,
                CharDriverState *chr)
{
    PXA2xxFIrState *s = (PXA2xxFIrState *)
            g_malloc0(sizeof(PXA2xxFIrState));

    s->irq = irq;
    s->rx_dma = rx_dma;
    s->tx_dma = tx_dma;
    s->chr = chr;

    pxa2xx_fir_reset(s);

    memory_region_init_io(&s->iomem, NULL, &pxa2xx_fir_ops, s, "pxa2xx-fir", 0x1000);
    memory_region_add_subregion(sysmem, base, &s->iomem);

    if (chr) {
        qemu_chr_fe_claim_no_fail(chr);
        qemu_chr_add_handlers(chr, pxa2xx_fir_is_empty,
                        pxa2xx_fir_rx, pxa2xx_fir_event, s);
    }

    register_savevm(NULL, "pxa2xx_fir", 0, 0, pxa2xx_fir_save,
                    pxa2xx_fir_load, s);

    return s;
}