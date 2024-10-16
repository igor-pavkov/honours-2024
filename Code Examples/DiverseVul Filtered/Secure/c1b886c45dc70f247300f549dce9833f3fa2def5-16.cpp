int vga_ioport_invalid(VGACommonState *s, uint32_t addr)
{
    if (s->msr & VGA_MIS_COLOR) {
        /* Color */
        return (addr >= 0x3b0 && addr <= 0x3bf);
    } else {
        /* Monochrome */
        return (addr >= 0x3d0 && addr <= 0x3df);
    }
}