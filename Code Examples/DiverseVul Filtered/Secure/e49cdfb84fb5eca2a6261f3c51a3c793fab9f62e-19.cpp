ok_png ok_png_read_with_allocator(FILE *file, ok_png_decode_flags decode_flags,
                                  ok_png_allocator allocator, void *allocator_user_data) {
    ok_png png = { 0 };
    if (file) {
        ok_png_decode(&png, decode_flags, OK_PNG_FILE_INPUT, file, allocator, allocator_user_data);
    } else {
        ok_png_error(&png, OK_PNG_ERROR_API, "File not found");
    }
    return png;
}