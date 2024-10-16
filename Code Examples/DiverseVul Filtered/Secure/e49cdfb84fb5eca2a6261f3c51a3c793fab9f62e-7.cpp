void ok_png_decode(ok_png *png, ok_png_decode_flags decode_flags,
                   ok_png_input input, void *input_user_data,
                   ok_png_allocator allocator, void *allocator_user_data) {
    if (!input.read || !input.seek) {
        ok_png_error(png, OK_PNG_ERROR_API,
                     "Invalid argument: input read and seek functions must not be NULL");
        return;
    }
    
    if (!allocator.alloc || !allocator.free) {
        ok_png_error(png, OK_PNG_ERROR_API,
                     "Invalid argument: allocator alloc and free functions must not be NULL");
        return;
    }

    ok_png_decoder *decoder = allocator.alloc(allocator_user_data, sizeof(ok_png_decoder));
    if (!decoder) {
        ok_png_error(png, OK_PNG_ERROR_ALLOCATION, "Couldn't allocate decoder.");
        return;
    }
    memset(decoder, 0, sizeof(ok_png_decoder));

    decoder->png = png;
    decoder->decode_flags = decode_flags;
    decoder->input = input;
    decoder->input_user_data = input_user_data;
    decoder->allocator = allocator;
    decoder->allocator_user_data = allocator_user_data;

    ok_png_decode2(decoder);

    // Cleanup decoder
    ok_inflater_free(decoder->inflater);
    allocator.free(allocator_user_data, decoder->curr_scanline);
    allocator.free(allocator_user_data, decoder->prev_scanline);
    allocator.free(allocator_user_data, decoder->inflate_buffer);
    allocator.free(allocator_user_data, decoder->temp_data_row);
    allocator.free(allocator_user_data, decoder);
}