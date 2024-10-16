ok_png ok_png_read(FILE *file, ok_png_decode_flags decode_flags) {
    return ok_png_read_with_allocator(file, decode_flags, OK_PNG_DEFAULT_ALLOCATOR, NULL);
}