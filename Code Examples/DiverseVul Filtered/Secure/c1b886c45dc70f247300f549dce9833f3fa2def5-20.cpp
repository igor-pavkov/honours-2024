static int vga_get_bpp(VGACommonState *s)
{
    int ret;

    if (s->vbe_regs[VBE_DISPI_INDEX_ENABLE] & VBE_DISPI_ENABLED) {
        ret = s->vbe_regs[VBE_DISPI_INDEX_BPP];
    } else {
        ret = 0;
    }
    return ret;
}