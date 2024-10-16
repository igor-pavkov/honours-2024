static Image *ReadPNGImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  printf("Your PNG library is too old: You have libpng-%s\n",
     PNG_LIBPNG_VER_STRING);

  (void) ThrowMagickException(exception,GetMagickModule(),CoderError,
    "PNG library is too old","`%s'",image_info->filename);

  return(Image *) NULL;
}