void vga_dirty_log_start(VGACommonState *s)
{
    memory_region_set_log(&s->vram, true, DIRTY_MEMORY_VGA);
}