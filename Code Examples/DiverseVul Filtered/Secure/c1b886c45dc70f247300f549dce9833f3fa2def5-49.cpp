static void vga_draw_blank(VGACommonState *s, int full_update)
{
    DisplaySurface *surface = qemu_console_surface(s->con);
    int i, w, val;
    uint8_t *d;

    if (!full_update)
        return;
    if (s->last_scr_width <= 0 || s->last_scr_height <= 0)
        return;

    s->rgb_to_pixel =
        rgb_to_pixel_dup_table[get_depth_index(surface)];
    if (surface_bits_per_pixel(surface) == 8) {
        val = s->rgb_to_pixel(0, 0, 0);
    } else {
        val = 0;
    }
    w = s->last_scr_width * surface_bytes_per_pixel(surface);
    d = surface_data(surface);
    for(i = 0; i < s->last_scr_height; i++) {
        memset(d, val, w);
        d += surface_stride(surface);
    }
    dpy_gfx_update(s->con, 0, 0,
                   s->last_scr_width, s->last_scr_height);
}