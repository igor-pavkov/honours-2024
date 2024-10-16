void vga_dirty_log_stop(VGACommonState *s)
{
    memory_region_set_log(&s->vram, false, DIRTY_MEMORY_VGA);
}