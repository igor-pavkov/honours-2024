static int vga_common_post_load(void *opaque, int version_id)
{
    VGACommonState *s = opaque;

    /* force refresh */
    s->graphic_mode = -1;
    return 0;
}