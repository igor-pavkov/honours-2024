static void vga_get_offsets(VGACommonState *s,
                            uint32_t *pline_offset,
                            uint32_t *pstart_addr,
                            uint32_t *pline_compare)
{
    uint32_t start_addr, line_offset, line_compare;

    if (s->vbe_regs[VBE_DISPI_INDEX_ENABLE] & VBE_DISPI_ENABLED) {
        line_offset = s->vbe_line_offset;
        start_addr = s->vbe_start_addr;
        line_compare = 65535;
    } else {
        /* compute line_offset in bytes */
        line_offset = s->cr[VGA_CRTC_OFFSET];
        line_offset <<= 3;

        /* starting address */
        start_addr = s->cr[VGA_CRTC_START_LO] |
            (s->cr[VGA_CRTC_START_HI] << 8);

        /* line compare */
        line_compare = s->cr[VGA_CRTC_LINE_COMPARE] |
            ((s->cr[VGA_CRTC_OVERFLOW] & 0x10) << 4) |
            ((s->cr[VGA_CRTC_MAX_SCAN] & 0x40) << 3);
    }
    *pline_offset = line_offset;
    *pstart_addr = start_addr;
    *pline_compare = line_compare;
}