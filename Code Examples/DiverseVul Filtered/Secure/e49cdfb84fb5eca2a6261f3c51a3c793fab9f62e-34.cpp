static void ok_png_transform_scanline(ok_png_decoder *decoder, const uint8_t *src, uint32_t width) {
    ok_png *png = decoder->png;
    const bool dst_flip_y = (decoder->decode_flags & OK_PNG_FLIP_Y) != 0;
    uint8_t *dst_start;
    uint8_t *dst_end;
    if (decoder->interlace_method == 0) {
        const uint32_t dst_y =
            (dst_flip_y ? (png->height - decoder->scanline - 1) : decoder->scanline);
        dst_start = png->data + (dst_y * png->stride);
    } else if (decoder->interlace_pass == 7) {
        const uint32_t t_scanline = decoder->scanline * 2 + 1;
        const uint32_t dst_y = dst_flip_y ? (png->height - t_scanline - 1) : t_scanline;
        dst_start = png->data + (dst_y * png->stride);
    } else {
        dst_start = decoder->temp_data_row;
    }
    dst_end = dst_start + width * png->bpp;

    const int c = decoder->color_type;
    const int d = decoder->bit_depth;
    const bool t = decoder->has_single_transparent_color;
    const bool has_full_alpha = (c == OK_PNG_COLOR_TYPE_GRAYSCALE_WITH_ALPHA ||
                                 c == OK_PNG_COLOR_TYPE_RGB_WITH_ALPHA);
    const bool src_is_premultiplied = decoder->is_ios_format;
    const bool dst_is_premultiplied = (decoder->decode_flags & OK_PNG_PREMULTIPLIED_ALPHA) != 0;
    const bool src_is_bgr = decoder->is_ios_format;
    const bool dst_is_bgr = (decoder->decode_flags & OK_PNG_COLOR_FORMAT_BGRA) != 0;
    bool should_byteswap = ((c == OK_PNG_COLOR_TYPE_RGB || c == OK_PNG_COLOR_TYPE_RGB_WITH_ALPHA) &&
                            src_is_bgr != dst_is_bgr);

    // Simple transforms
    if (c == OK_PNG_COLOR_TYPE_GRAYSCALE && d == 8 && !t) {
        uint8_t *dst = dst_start;
        while (dst < dst_end) {
            uint8_t v = *src++;
            *dst++ = v;
            *dst++ = v;
            *dst++ = v;
            *dst++ = 0xff;
        }
    } else if (c == OK_PNG_COLOR_TYPE_PALETTE && d == 8) {
        uint8_t *dst = dst_start;
        const uint8_t *palette = decoder->palette;
        while (dst < dst_end) {
            memcpy(dst, palette + *src * 4, 4);
            dst += 4;
            src++;
        }
    } else if (c == OK_PNG_COLOR_TYPE_RGB && d == 8 && !t) {
        if (should_byteswap) {
            uint8_t *dst = dst_start;
            while (dst < dst_end) {
                dst[0] = src[2];
                dst[1] = src[1];
                dst[2] = src[0];
                dst[3] = 0xff;
                src += 3;
                dst += 4;
            }
            should_byteswap = false;
        } else {
            uint8_t *dst = dst_start;
            while (dst < dst_end) {
                memcpy(dst, src, 3);
                dst[3] = 0xff;
                src += 3;
                dst += 4;
            }
        }
    } else if (c == OK_PNG_COLOR_TYPE_GRAYSCALE_WITH_ALPHA && d == 8) {
        uint8_t *dst = dst_start;
        while (dst < dst_end) {
            uint8_t v = *src++;
            uint8_t a = *src++;
            *dst++ = v;
            *dst++ = v;
            *dst++ = v;
            *dst++ = a;
        }
    } else if (c == OK_PNG_COLOR_TYPE_RGB_WITH_ALPHA && d == 8) {
        memcpy(dst_start, src, width * 4);
    } else {
        // Complex transforms: 1-, 2-, 4- and 16-bit, and 8-bit with single-color transparency.
        const uint8_t *palette = decoder->palette;
        const int bitmask = (1 << d) - 1;
        int bit = 8 - d;
        uint16_t tr = decoder->single_transparent_color_key[0];
        uint16_t tg = decoder->single_transparent_color_key[1];
        uint16_t tb = decoder->single_transparent_color_key[2];
        if (d <= 8) {
            tr = (uint16_t)((tr & bitmask) * (255 / bitmask));
            tg = (uint16_t)((tg & bitmask) * (255 / bitmask));
            tb = (uint16_t)((tb & bitmask) * (255 / bitmask));
        }
        uint8_t *dst = dst_start;
        while (dst < dst_end) {
            uint16_t r = 0;
            uint16_t g = 0;
            uint16_t b = 0;
            uint16_t a = 0xffff;

            if (d < 8) {
                if (bit < 0) {
                    bit = 8 - d;
                    src++;
                }
                int v = (*src >> bit) & bitmask;
                if (c == OK_PNG_COLOR_TYPE_GRAYSCALE) {
                    r = g = b = (uint16_t)(v * (255 / bitmask));
                } else {
                    const uint8_t *psrc = palette + (v * 4);
                    r = *psrc++;
                    g = *psrc++;
                    b = *psrc++;
                    a = *psrc++;
                }
                bit -= d;
            } else if (d == 8) {
                if (c == OK_PNG_COLOR_TYPE_GRAYSCALE) {
                    r = g = b = *src++;
                } else if (c == OK_PNG_COLOR_TYPE_PALETTE) {
                    const uint8_t *psrc = palette + (*src++ * 4);
                    r = *psrc++;
                    g = *psrc++;
                    b = *psrc++;
                    a = *psrc++;
                } else if (c == OK_PNG_COLOR_TYPE_GRAYSCALE_WITH_ALPHA) {
                    r = g = b = *src++;
                    a = *src++;
                } else if (c == OK_PNG_COLOR_TYPE_RGB) {
                    r = *src++;
                    g = *src++;
                    b = *src++;
                } else if (c == OK_PNG_COLOR_TYPE_RGB_WITH_ALPHA) {
                    r = *src++;
                    g = *src++;
                    b = *src++;
                    a = *src++;
                }
            } else if (d == 16) {
                if (c == OK_PNG_COLOR_TYPE_GRAYSCALE) {
                    r = g = b = readBE16(src);
                    src += 2;
                } else if (c == OK_PNG_COLOR_TYPE_GRAYSCALE_WITH_ALPHA) {
                    r = g = b = readBE16(src);
                    a = readBE16(src + 2);
                    src += 4;
                } else if (c == OK_PNG_COLOR_TYPE_RGB) {
                    r = readBE16(src);
                    g = readBE16(src + 2);
                    b = readBE16(src + 4);
                    src += 6;
                } else if (c == OK_PNG_COLOR_TYPE_RGB_WITH_ALPHA) {
                    r = readBE16(src);
                    g = readBE16(src + 2);
                    b = readBE16(src + 4);
                    a = readBE16(src + 6);
                    src += 8;
                }
            }

            if (t && r == tr && g == tg && b == tb) {
                a = 0;
                if (dst_is_premultiplied) {
                    r = b = g = 0;
                }
            }

            if (d == 16) {
                // This is libpng's formula for scaling 16-bit to 8-bit
                r = (r * 255 + 32895) >> 16;
                g = (g * 255 + 32895) >> 16;
                b = (b * 255 + 32895) >> 16;
                a = (a * 255 + 32895) >> 16;
            }

            if (should_byteswap) {
                *dst++ = (uint8_t)b;
                *dst++ = (uint8_t)g;
                *dst++ = (uint8_t)r;
                *dst++ = (uint8_t)a;
            } else {
                *dst++ = (uint8_t)r;
                *dst++ = (uint8_t)g;
                *dst++ = (uint8_t)b;
                *dst++ = (uint8_t)a;
            }
        }
        should_byteswap = false;
    }

    // Color format convert: Premultiply, un-premultiply, and swap if needed
    if (should_byteswap) {
        if (has_full_alpha && src_is_premultiplied && !dst_is_premultiplied) {
            // Convert from BGRA_PRE to RGBA
            for (uint8_t *dst = dst_start; dst < dst_end; dst += 4) {
                const uint8_t v = dst[0];
                dst[0] = dst[2];
                dst[2] = v;
                ok_png_unpremultiply(dst);
            }
        } else if (has_full_alpha && !src_is_premultiplied && dst_is_premultiplied) {
            // Convert from BGRA to RGBA_PRE
            for (uint8_t *dst = dst_start; dst < dst_end; dst += 4) {
                const uint8_t v = dst[0];
                dst[0] = dst[2];
                dst[2] = v;
                ok_png_premultiply(dst);
            }
        } else {
            // Convert from BGRA to RGBA (or BGRA_PRE to RGBA_PRE)
            for (uint8_t *dst = dst_start; dst < dst_end; dst += 4) {
                const uint8_t v = dst[0];
                dst[0] = dst[2];
                dst[2] = v;
            }
        }
    } else if (has_full_alpha) {
        if (src_is_premultiplied && !dst_is_premultiplied) {
            // Convert from RGBA_PRE to RGBA
            for (uint8_t *dst = dst_start; dst < dst_end; dst += 4) {
                ok_png_unpremultiply(dst);
            }
        } else if (!src_is_premultiplied && dst_is_premultiplied) {
            // Convert from RGBA to RGBA_PRE
            for (uint8_t *dst = dst_start; dst < dst_end; dst += 4) {
                ok_png_premultiply(dst);
            }
        } else {
            // Do nothing: Already in correct format, RGBA or RGBA_PRE
        }
    }

    // If interlaced, copy from the temp buffer
    if (decoder->interlace_method == 1 && decoder->interlace_pass < 7) {
        const int i = decoder->interlace_pass;
        const uint32_t s = decoder->scanline;
        //                                       1      2      3      4      5      6      7
        static const uint32_t dst_x[]  = {0,     0,     4,     0,     2,     0,     1,     0 };
        static const uint32_t dst_dx[] = {0,     8,     8,     4,     4,     2,     2,     1 };
               const uint32_t dst_y[]  = {0,   s*8,   s*8, 4+s*8,   s*4, 2+s*4,   s*2, 1+s*2 };

        uint32_t x = dst_x[i];
        uint32_t y = dst_y[i];
        uint32_t dx = 4 * dst_dx[i];
        if (dst_flip_y) {
            y = (png->height - y - 1);
        }

        src = dst_start;
        uint8_t *src_end = dst_end;
        uint8_t *dst = png->data + (y * png->stride) + (x * 4);
        while (src < src_end) {
            memcpy(dst, src, 4);
            dst += dx;
            src += 4;
        }
    }
}