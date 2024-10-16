static png_free_ptr Magick_png_free(png_structp png_ptr,png_voidp ptr)
{
  (void) png_ptr;
  ptr=RelinquishMagickMemory(ptr);
  return((png_free_ptr) NULL);
}