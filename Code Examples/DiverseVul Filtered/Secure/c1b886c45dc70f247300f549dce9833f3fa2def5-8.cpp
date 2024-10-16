void vga_sync_dirty_bitmap(VGACommonState *s)
{
    memory_region_sync_dirty_bitmap(&s->vram);
}