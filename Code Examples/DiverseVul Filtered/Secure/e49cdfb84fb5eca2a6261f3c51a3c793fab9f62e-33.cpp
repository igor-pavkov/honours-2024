static bool ok_png_read_header(ok_png_decoder *decoder, uint32_t chunk_length) {
    ok_png *png = decoder->png;
    if (chunk_length != 13) {
        ok_png_error(png, OK_PNG_ERROR_INVALID, "Invalid IHDR chunk length");
        return false;
    }
    uint8_t chunk_data[13];
    if (!ok_read(decoder, chunk_data, sizeof(chunk_data))) {
        return false;
    }
    png->width = readBE32(chunk_data);
    png->height = readBE32(chunk_data + 4);
    png->bpp = 4; // Always decoding to 32-bit color
    decoder->bit_depth = chunk_data[8];
    decoder->color_type = chunk_data[9];
    uint8_t compression_method = chunk_data[10];
    uint8_t filter_method = chunk_data[11];
    decoder->interlace_method = chunk_data[12];
    uint64_t stride = (uint64_t)png->width * png->bpp;

    if (compression_method != 0) {
        ok_png_error(png, OK_PNG_ERROR_INVALID, "Invalid compression method");
        return false;
    } else if (filter_method != 0) {
        ok_png_error(png, OK_PNG_ERROR_INVALID, "Invalid filter method");
        return false;
    } else if (decoder->interlace_method != 0 && decoder->interlace_method != 1) {
        ok_png_error(png, OK_PNG_ERROR_INVALID, "Invalid interlace method");
        return false;
    } else if (stride > UINT32_MAX) {
        ok_png_error(png, OK_PNG_ERROR_UNSUPPORTED, "Width too large");
        return false;
    }

    const int c = decoder->color_type;
    const int b = decoder->bit_depth;
    const bool valid =
        (c == OK_PNG_COLOR_TYPE_GRAYSCALE && (b == 1 || b == 2 || b == 4 || b == 8 || b == 16)) ||
        (c == OK_PNG_COLOR_TYPE_RGB && (b == 8 || b == 16)) ||
        (c == OK_PNG_COLOR_TYPE_PALETTE && (b == 1 || b == 2 || b == 4 || b == 8)) ||
        (c == OK_PNG_COLOR_TYPE_GRAYSCALE_WITH_ALPHA && (b == 8 || b == 16)) ||
        (c == OK_PNG_COLOR_TYPE_RGB_WITH_ALPHA && (b == 8 || b == 16));

    if (!valid) {
        ok_png_error(png, OK_PNG_ERROR_INVALID, "Invalid combination of color type and bit depth");
        return false;
    }

    png->stride = (uint32_t)stride;
    png->has_alpha = (c == OK_PNG_COLOR_TYPE_GRAYSCALE_WITH_ALPHA ||
                      c == OK_PNG_COLOR_TYPE_RGB_WITH_ALPHA);
    decoder->interlace_pass = 0;
    decoder->ready_for_next_interlace_pass = true;
    return true;
}