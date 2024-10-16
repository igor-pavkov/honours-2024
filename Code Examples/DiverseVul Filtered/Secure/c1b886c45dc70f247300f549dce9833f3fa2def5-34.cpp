static int update_palette256(VGACommonState *s)
{
    int full_update, i;
    uint32_t v, col, *palette;

    full_update = 0;
    palette = s->last_palette;
    v = 0;
    for(i = 0; i < 256; i++) {
        if (s->dac_8bit) {
          col = s->rgb_to_pixel(s->palette[v],
                                s->palette[v + 1],
                                s->palette[v + 2]);
        } else {
          col = s->rgb_to_pixel(c6_to_8(s->palette[v]),
                                c6_to_8(s->palette[v + 1]),
                                c6_to_8(s->palette[v + 2]));
        }
        if (col != palette[i]) {
            full_update = 1;
            palette[i] = col;
        }
        v += 3;
    }
    return full_update;
}