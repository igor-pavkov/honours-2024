static void vbe_fixup_regs(VGACommonState *s)
{
    uint16_t *r = s->vbe_regs;
    uint32_t bits, linelength, maxy, offset;

    if (!(r[VBE_DISPI_INDEX_ENABLE] & VBE_DISPI_ENABLED)) {
        /* vbe is turned off -- nothing to do */
        return;
    }

    /* check depth */
    switch (r[VBE_DISPI_INDEX_BPP]) {
    case 4:
    case 8:
    case 16:
    case 24:
    case 32:
        bits = r[VBE_DISPI_INDEX_BPP];
        break;
    case 15:
        bits = 16;
        break;
    default:
        bits = r[VBE_DISPI_INDEX_BPP] = 8;
        break;
    }

    /* check width */
    r[VBE_DISPI_INDEX_XRES] &= ~7u;
    if (r[VBE_DISPI_INDEX_XRES] == 0) {
        r[VBE_DISPI_INDEX_XRES] = 8;
    }
    if (r[VBE_DISPI_INDEX_XRES] > VBE_DISPI_MAX_XRES) {
        r[VBE_DISPI_INDEX_XRES] = VBE_DISPI_MAX_XRES;
    }
    r[VBE_DISPI_INDEX_VIRT_WIDTH] &= ~7u;
    if (r[VBE_DISPI_INDEX_VIRT_WIDTH] > VBE_DISPI_MAX_XRES) {
        r[VBE_DISPI_INDEX_VIRT_WIDTH] = VBE_DISPI_MAX_XRES;
    }
    if (r[VBE_DISPI_INDEX_VIRT_WIDTH] < r[VBE_DISPI_INDEX_XRES]) {
        r[VBE_DISPI_INDEX_VIRT_WIDTH] = r[VBE_DISPI_INDEX_XRES];
    }

    /* check height */
    linelength = r[VBE_DISPI_INDEX_VIRT_WIDTH] * bits / 8;
    maxy = s->vbe_size / linelength;
    if (r[VBE_DISPI_INDEX_YRES] == 0) {
        r[VBE_DISPI_INDEX_YRES] = 1;
    }
    if (r[VBE_DISPI_INDEX_YRES] > VBE_DISPI_MAX_YRES) {
        r[VBE_DISPI_INDEX_YRES] = VBE_DISPI_MAX_YRES;
    }
    if (r[VBE_DISPI_INDEX_YRES] > maxy) {
        r[VBE_DISPI_INDEX_YRES] = maxy;
    }

    /* check offset */
    if (r[VBE_DISPI_INDEX_X_OFFSET] > VBE_DISPI_MAX_XRES) {
        r[VBE_DISPI_INDEX_X_OFFSET] = VBE_DISPI_MAX_XRES;
    }
    if (r[VBE_DISPI_INDEX_Y_OFFSET] > VBE_DISPI_MAX_YRES) {
        r[VBE_DISPI_INDEX_Y_OFFSET] = VBE_DISPI_MAX_YRES;
    }
    offset = r[VBE_DISPI_INDEX_X_OFFSET] * bits / 8;
    offset += r[VBE_DISPI_INDEX_Y_OFFSET] * linelength;
    if (offset + r[VBE_DISPI_INDEX_YRES] * linelength > s->vbe_size) {
        r[VBE_DISPI_INDEX_Y_OFFSET] = 0;
        offset = r[VBE_DISPI_INDEX_X_OFFSET] * bits / 8;
        if (offset + r[VBE_DISPI_INDEX_YRES] * linelength > s->vbe_size) {
            r[VBE_DISPI_INDEX_X_OFFSET] = 0;
            offset = 0;
        }
    }

    /* update vga state */
    r[VBE_DISPI_INDEX_VIRT_HEIGHT] = maxy;
    s->vbe_line_offset = linelength;
    s->vbe_start_addr  = offset / 4;
}