static bool ok_png_read_palette(ok_png_decoder *decoder, uint32_t chunk_length) {
    ok_png *png = decoder->png;
    decoder->palette_length = chunk_length / 3;

    if (decoder->palette_length > 256 || decoder->palette_length * 3 != chunk_length) {
        ok_png_error(png, OK_PNG_ERROR_INVALID, "Invalid palette chunk length");
        return false;
    }
    const bool src_is_bgr = decoder->is_ios_format;
    const bool dst_is_bgr = (decoder->decode_flags & OK_PNG_COLOR_FORMAT_BGRA) != 0;
    const bool should_byteswap = src_is_bgr != dst_is_bgr;
    uint8_t *dst = decoder->palette;
    uint8_t buffer[256 * 3];
    if (!ok_read(decoder, buffer, 3 * decoder->palette_length)) {
        return false;
    }
    uint8_t *in = buffer;
    if (should_byteswap) {
        for (uint32_t i = 0; i < decoder->palette_length; i++, in += 3, dst += 4) {
            dst[0] = in[2];
            dst[1] = in[1];
            dst[2] = in[0];
            dst[3] = 0xff;
        }
    } else {
        for (uint32_t i = 0; i < decoder->palette_length; i++, in += 3, dst += 4) {
            dst[0] = in[0];
            dst[1] = in[1];
            dst[2] = in[2];
            dst[3] = 0xff;
        }
    }
    return true;
}