static void ssd0323_update_display(void *opaque)
{
    ssd0323_state *s = (ssd0323_state *)opaque;
    DisplaySurface *surface = qemu_console_surface(s->con);
    uint8_t *dest;
    uint8_t *src;
    int x;
    int y;
    int i;
    int line;
    char *colors[16];
    char colortab[MAGNIFY * 64];
    char *p;
    int dest_width;

    if (!s->redraw)
        return;

    switch (surface_bits_per_pixel(surface)) {
    case 0:
        return;
    case 15:
        dest_width = 2;
        break;
    case 16:
        dest_width = 2;
        break;
    case 24:
        dest_width = 3;
        break;
    case 32:
        dest_width = 4;
        break;
    default:
        BADF("Bad color depth\n");
        return;
    }
    p = colortab;
    for (i = 0; i < 16; i++) {
        int n;
        colors[i] = p;
        switch (surface_bits_per_pixel(surface)) {
        case 15:
            n = i * 2 + (i >> 3);
            p[0] = n | (n << 5);
            p[1] = (n << 2) | (n >> 3);
            break;
        case 16:
            n = i * 2 + (i >> 3);
            p[0] = n | (n << 6) | ((n << 1) & 0x20);
            p[1] = (n << 3) | (n >> 2);
            break;
        case 24:
        case 32:
            n = (i << 4) | i;
            p[0] = p[1] = p[2] = n;
            break;
        default:
            BADF("Bad color depth\n");
            return;
        }
        p += dest_width;
    }
    /* TODO: Implement row/column remapping.  */
    dest = surface_data(surface);
    for (y = 0; y < 64; y++) {
        line = y;
        src = s->framebuffer + 64 * line;
        for (x = 0; x < 64; x++) {
            int val;
            val = *src >> 4;
            for (i = 0; i < MAGNIFY; i++) {
                memcpy(dest, colors[val], dest_width);
                dest += dest_width;
            }
            val = *src & 0xf;
            for (i = 0; i < MAGNIFY; i++) {
                memcpy(dest, colors[val], dest_width);
                dest += dest_width;
            }
            src++;
        }
        for (i = 1; i < MAGNIFY; i++) {
            memcpy(dest, dest - dest_width * MAGNIFY * 128,
                   dest_width * 128 * MAGNIFY);
            dest += dest_width * 128 * MAGNIFY;
        }
    }
    s->redraw = 0;
    dpy_gfx_update(s->con, 0, 0, 128 * MAGNIFY, 64 * MAGNIFY);
}