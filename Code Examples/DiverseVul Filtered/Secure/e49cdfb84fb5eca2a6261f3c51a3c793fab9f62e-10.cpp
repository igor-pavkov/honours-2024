static bool ok_png_read_transparency(ok_png_decoder *decoder, uint32_t chunk_length) {
    ok_png *png = decoder->png;
    png->has_alpha = true;

    if (decoder->color_type == OK_PNG_COLOR_TYPE_PALETTE) {
        if (chunk_length > decoder->palette_length || chunk_length > 256) {
            ok_png_error(png, OK_PNG_ERROR_INVALID,
                         "Invalid transparency length for palette color type");
            return false;
        }

        const bool should_premultiply = (decoder->decode_flags & OK_PNG_PREMULTIPLIED_ALPHA) != 0;
        uint8_t *dst = decoder->palette;
        uint8_t buffer[256];
        if (!ok_read(decoder, buffer, chunk_length)) {
            return false;
        }
        for (uint32_t i = 0; i < chunk_length; i++) {
            dst[3] = buffer[i];
            if (should_premultiply) {
                ok_png_premultiply(dst);
            }
            dst += 4;
        }
        return true;
    } else if (decoder->color_type == OK_PNG_COLOR_TYPE_GRAYSCALE) {
        if (chunk_length != 2) {
            ok_png_error(png, OK_PNG_ERROR_INVALID,
                         "Invalid transparency length for grayscale color type");
            return false;
        } else {
            uint8_t buffer[2];
            if (!ok_read(decoder, buffer, sizeof(buffer))) {
                return false;
            }
            const uint16_t v = readBE16(buffer);
            decoder->single_transparent_color_key[0] = v;
            decoder->single_transparent_color_key[1] = v;
            decoder->single_transparent_color_key[2] = v;
            decoder->has_single_transparent_color = true;
            return true;
        }
    } else if (decoder->color_type == OK_PNG_COLOR_TYPE_RGB) {
        if (chunk_length != 6) {
            ok_png_error(png, OK_PNG_ERROR_INVALID,
                         "Invalid transparency length for truecolor color type");
            return false;
        } else {
            uint8_t buffer[6];
            if (!ok_read(decoder, buffer, sizeof(buffer))) {
                return false;
            }
            decoder->single_transparent_color_key[0] = readBE16(buffer + 0);
            decoder->single_transparent_color_key[1] = readBE16(buffer + 2);
            decoder->single_transparent_color_key[2] = readBE16(buffer + 4);
            decoder->has_single_transparent_color = true;
            return true;
        }
    } else {
        ok_png_error(png, OK_PNG_ERROR_INVALID,
                     "Invalid transparency for color type");
        return false;
    }
}