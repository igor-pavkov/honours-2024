static void vga_update_display(void *opaque)
{
    VGACommonState *s = opaque;
    DisplaySurface *surface = qemu_console_surface(s->con);
    int full_update, graphic_mode;

    qemu_flush_coalesced_mmio_buffer();

    if (surface_bits_per_pixel(surface) == 0) {
        /* nothing to do */
    } else {
        full_update = 0;
        if (!(s->ar_index & 0x20)) {
            graphic_mode = GMODE_BLANK;
        } else {
            graphic_mode = s->gr[VGA_GFX_MISC] & VGA_GR06_GRAPHICS_MODE;
        }
        if (graphic_mode != s->graphic_mode) {
            s->graphic_mode = graphic_mode;
            s->cursor_blink_time = qemu_clock_get_ms(QEMU_CLOCK_VIRTUAL);
            full_update = 1;
        }
        switch(graphic_mode) {
        case GMODE_TEXT:
            vga_draw_text(s, full_update);
            break;
        case GMODE_GRAPH:
            vga_draw_graphic(s, full_update);
            break;
        case GMODE_BLANK:
        default:
            vga_draw_blank(s, full_update);
            break;
        }
    }
}