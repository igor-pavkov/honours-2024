static bool ok_png_read_data(ok_png_decoder *decoder, uint32_t bytes_remaining) {
    ok_png *png = decoder->png;
    size_t inflate_buffer_size = 64 * 1024;
    size_t num_passes = decoder->interlace_method == 0 ? 1 : 7;
    uint8_t bits_per_pixel = decoder->bit_depth * OK_PNG_SAMPLES_PER_PIXEL[decoder->color_type];
    uint8_t bytes_per_pixel = (bits_per_pixel + 7) / 8;
    uint64_t max_bytes_per_scanline = 1 + ((uint64_t)png->width * bits_per_pixel + 7) / 8;
    size_t platform_max_bytes_per_scanline = (size_t)max_bytes_per_scanline;

    // Create buffers
    if (!png->data) {
        if (decoder->allocator.image_alloc) {
            decoder->allocator.image_alloc(decoder->allocator_user_data,
                                           png->width, png->height, png->bpp,
                                           &png->data, &png->stride);
        } else {
            uint64_t size = (uint64_t)png->stride * png->height;
            size_t platform_size = (size_t)size;
            if (platform_size == size) {
                png->data = ok_alloc(decoder, platform_size);
            }
        }
        if (!png->data) {
            ok_png_error(png, OK_PNG_ERROR_ALLOCATION, "Couldn't allocate memory for image");
            return false;
        }
        if (png->stride < png->width * png->bpp) {
            ok_png_error(png, OK_PNG_ERROR_API, "Invalid stride");
            return false;
        }
    }
    if (!decoder->prev_scanline) {
        if (max_bytes_per_scanline == platform_max_bytes_per_scanline) {
            decoder->prev_scanline = ok_alloc(decoder, platform_max_bytes_per_scanline);
        }
    }
    if (!decoder->curr_scanline) {
        if (max_bytes_per_scanline == platform_max_bytes_per_scanline) {
            decoder->curr_scanline = ok_alloc(decoder, platform_max_bytes_per_scanline);
        }
    }
    if (!decoder->inflate_buffer) {
        decoder->inflate_buffer = ok_alloc(decoder, inflate_buffer_size);
    }
    if (decoder->interlace_method == 1 && !decoder->temp_data_row) {
        decoder->temp_data_row = ok_alloc(decoder, png->width * png->bpp);
    }
    if (!decoder->curr_scanline || !decoder->prev_scanline || !decoder->inflate_buffer ||
        (decoder->interlace_method == 1 && !decoder->temp_data_row)) {
        ok_png_error(png, OK_PNG_ERROR_ALLOCATION, "Couldn't allocate buffers");
        return false;
    }

    // Setup inflater
    if (!decoder->inflater) {
        decoder->inflater = ok_inflater_init(decoder->is_ios_format,
                                             decoder->allocator, decoder->allocator_user_data);
        if (!decoder->inflater) {
            ok_png_error(png, OK_PNG_ERROR_ALLOCATION, "Couldn't init inflater");
            return false;
        }
    }

    // Sanity check - this happened with one file in the PNG suite
    if (decoder->decoding_completed) {
        if (bytes_remaining > 0) {
            return ok_seek(decoder, (long)bytes_remaining);
        } else {
            return true;
        }
    }

    // Read data
    uint32_t curr_width = ok_png_get_width_for_pass(decoder);
    uint32_t curr_height = ok_png_get_height_for_pass(decoder);
    size_t curr_bytes_per_scanline = (size_t)(1 + ((uint64_t)curr_width * bits_per_pixel + 7) / 8);
    while (true) {
        // Setup pass
        while (decoder->ready_for_next_interlace_pass) {
            decoder->ready_for_next_interlace_pass = false;
            decoder->scanline = 0;
            decoder->interlace_pass++;
            if (decoder->interlace_pass == num_passes + 1) {
                // Done decoding - skip any remaining chunk data
                decoder->decoding_completed = true;
                if (bytes_remaining > 0) {
                    return ok_seek(decoder, (long)bytes_remaining);
                } else {
                    return true;
                }
            }
            curr_width = ok_png_get_width_for_pass(decoder);
            curr_height = ok_png_get_height_for_pass(decoder);
            curr_bytes_per_scanline = (size_t)(1 + ((uint64_t)curr_width * bits_per_pixel + 7) / 8);
            if (curr_width == 0 || curr_height == 0) {
                // No data for this pass - happens if width or height <= 4
                decoder->ready_for_next_interlace_pass = true;
            } else {
                memset(decoder->curr_scanline, 0, curr_bytes_per_scanline);
                memset(decoder->prev_scanline, 0, curr_bytes_per_scanline);
                decoder->inflater_bytes_read = 0;
            }
        }

        // Read compressed data
        if (ok_inflater_needs_input(decoder->inflater)) {
            if (bytes_remaining == 0) {
                // Need more data, but there is no remaining data in this chunk.
                // There may be another IDAT chunk.
                return true;
            }
            const size_t len = min(inflate_buffer_size, bytes_remaining);
            if (!ok_read(decoder, decoder->inflate_buffer, len)) {
                return false;
            }
            bytes_remaining -= len;
            ok_inflater_set_input(decoder->inflater, decoder->inflate_buffer, len);
        }

        // Decompress data
        size_t len = ok_inflater_inflate(decoder->inflater,
                                         decoder->curr_scanline + decoder->inflater_bytes_read,
                                         curr_bytes_per_scanline - decoder->inflater_bytes_read);
        if (len == OK_SIZE_MAX) {
            ok_png_error(png, OK_PNG_ERROR_INFLATER, "Inflater error");
            return false;
        }
        decoder->inflater_bytes_read += len;
        if (decoder->inflater_bytes_read == curr_bytes_per_scanline) {
            // Apply filter
            const int filter = decoder->curr_scanline[0];
            if (filter > 0 && filter < OK_PNG_NUM_FILTERS) {
                ok_png_decode_filter(decoder->curr_scanline + 1, decoder->prev_scanline + 1,
                                     curr_bytes_per_scanline - 1, filter, bytes_per_pixel);
            } else if (filter != 0) {
                ok_png_error(png, OK_PNG_ERROR_INVALID, "Invalid filter type");
                return false;
            }

            // Transform
            ok_png_transform_scanline(decoder, decoder->curr_scanline + 1, curr_width);

            // Setup for next scanline or pass
            decoder->scanline++;
            if (decoder->scanline == curr_height) {
                decoder->ready_for_next_interlace_pass = true;
            } else {
                uint8_t *temp = decoder->curr_scanline;
                decoder->curr_scanline = decoder->prev_scanline;
                decoder->prev_scanline = temp;
                decoder->inflater_bytes_read = 0;
            }
        }
    }
}