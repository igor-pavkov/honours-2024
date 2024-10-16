int processing_data(png_structp png_ptr, png_infop info_ptr, unsigned char* p,
                    unsigned int size) {
  if (!png_ptr || !info_ptr) return 1;

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, 0);
    return 1;
  }

  png_process_data(png_ptr, info_ptr, p, size);
  return 0;
}