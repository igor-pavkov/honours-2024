static png_voidp Magick_png_malloc(png_structp png_ptr,png_size_t size)
#endif
{
  (void) png_ptr;
  return((png_voidp) AcquireMagickMemory((size_t) size));
}