void ff_h264_hl_decode_mb(H264Context *h)
{
    const int mb_xy   = h->mb_xy;
    const int mb_type = h->cur_pic.mb_type[mb_xy];
    int is_complex    = CONFIG_SMALL || h->is_complex ||
                        IS_INTRA_PCM(mb_type) || h->qscale == 0;

    if (CHROMA444(h)) {
        if (is_complex || h->pixel_shift)
            hl_decode_mb_444_complex(h);
        else
            hl_decode_mb_444_simple_8(h);
    } else if (is_complex) {
        hl_decode_mb_complex(h);
    } else if (h->pixel_shift) {
        hl_decode_mb_simple_16(h);
    } else
        hl_decode_mb_simple_8(h);
}