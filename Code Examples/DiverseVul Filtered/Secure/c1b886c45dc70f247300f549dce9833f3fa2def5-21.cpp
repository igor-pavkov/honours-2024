void vga_init_vbe(VGACommonState *s, Object *obj, MemoryRegion *system_memory)
{
    /* With pc-0.12 and below we map both the PCI BAR and the fixed VBE region,
     * so use an alias to avoid double-mapping the same region.
     */
    memory_region_init_alias(&s->vram_vbe, obj, "vram.vbe",
                             &s->vram, 0, memory_region_size(&s->vram));
    /* XXX: use optimized standard vga accesses */
    memory_region_add_subregion(system_memory,
                                VBE_DISPI_LFB_PHYSICAL_ADDRESS,
                                &s->vram_vbe);
    s->vbe_mapped = 1;
}