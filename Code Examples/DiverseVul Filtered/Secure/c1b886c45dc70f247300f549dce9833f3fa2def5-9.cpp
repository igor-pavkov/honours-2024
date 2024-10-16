static void vga_reset(void *opaque)
{
    VGACommonState *s =  opaque;
    vga_common_reset(s);
}