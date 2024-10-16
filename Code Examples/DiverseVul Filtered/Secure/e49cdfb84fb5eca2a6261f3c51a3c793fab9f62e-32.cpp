static uint32_t ok_png_get_height_for_pass(const ok_png_decoder *decoder) {
    const uint32_t h = decoder->png->height;
    if (decoder->interlace_method == 0) {
        return h;
    }

    switch (decoder->interlace_pass) {
        case 1: return (h + 7) / 8;
        case 2: return (h + 7) / 8;
        case 3: return (h + 3) / 8;
        case 4: return (h + 3) / 4;
        case 5: return (h + 1) / 4;
        case 6: return (h + 1) / 2;
        case 7: return h / 2;
        default: return 0;
    }
}