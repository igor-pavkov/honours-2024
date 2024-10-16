ok_png ok_png_read_from_input(ok_png_decode_flags decode_flags,
                              ok_png_input input_callbacks, void *input_callbacks_user_data,
                              ok_png_allocator allocator, void *allocator_user_data) {
    ok_png png = { 0 };
    ok_png_decode(&png, decode_flags, input_callbacks, input_callbacks_user_data,
                  allocator, allocator_user_data);
    return png;
}