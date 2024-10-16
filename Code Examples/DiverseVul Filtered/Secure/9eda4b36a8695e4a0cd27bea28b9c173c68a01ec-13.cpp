static void MagickPNGErrorHandler(png_struct *ping,png_const_charp message)
{
  Image
    *image;

  image=(Image *) png_get_error_ptr(ping);

  if (image->debug != MagickFalse)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
      "  libpng-%s error: %s", PNG_LIBPNG_VER_STRING,message);

  (void) ThrowMagickException(&image->exception,GetMagickModule(),CoderError,
    message,"`%s'",image->filename);

#if (PNG_LIBPNG_VER < 10500)
  /* A warning about deprecated use of jmpbuf here is unavoidable if you
   * are building with libpng-1.4.x and can be ignored.
   */
  longjmp(ping->jmpbuf,1);
#else
  png_longjmp(ping,1);
#endif
}