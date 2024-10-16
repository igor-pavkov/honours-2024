static MagickBooleanType WritePNGImage(const ImageInfo *image_info,Image *image)
{
  (void) image;
  printf("Your PNG library is too old: You have libpng-%s\n",
     PNG_LIBPNG_VER_STRING);

  ThrowBinaryException(CoderError,"PNG library is too old",
     image_info->filename);
}