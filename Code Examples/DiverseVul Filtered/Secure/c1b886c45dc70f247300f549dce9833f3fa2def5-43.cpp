static void vga_get_resolution(VGACommonState *s, int *pwidth, int *pheight)
{
    int width, height;

    if (s->vbe_regs[VBE_DISPI_INDEX_ENABLE] & VBE_DISPI_ENABLED) {
        width = s->vbe_regs[VBE_DISPI_INDEX_XRES];
        height = s->vbe_regs[VBE_DISPI_INDEX_YRES];
    } else {
        width = (s->cr[VGA_CRTC_H_DISP] + 1) * 8;
        height = s->cr[VGA_CRTC_V_DISP_END] |
            ((s->cr[VGA_CRTC_OVERFLOW] & 0x02) << 7) |
            ((s->cr[VGA_CRTC_OVERFLOW] & 0x40) << 3);
        height = (height + 1);
    }
    *pwidth = width;
    *pheight = height;
}