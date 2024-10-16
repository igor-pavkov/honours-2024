static uint32_t ok_png_get_width_for_pass(const ok_png_decoder *decoder) {
    const uint32_t w = decoder->png->width;
    if (decoder->interlace_method == 0) {
        return w;
    }

    switch (decoder->interlace_pass) {
        case 1: return (w + 7) / 8;
        case 2: return (w + 3) / 8;
        case 3: return (w + 3) / 4;
        case 4: return (w + 1) / 4;
        case 5: return (w + 1) / 2;
        case 6: return w / 2;
        case 7: return w;
        default: return 0;
    }
}