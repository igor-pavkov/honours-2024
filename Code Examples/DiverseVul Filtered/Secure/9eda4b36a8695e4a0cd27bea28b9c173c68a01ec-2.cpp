static void MagickPNGWarningHandler(png_struct *ping,png_const_charp message)
{
  Image
    *image;

  if (LocaleCompare(message, "Missing PLTE before tRNS") == 0)
    png_error(ping, message);

  image=(Image *) png_get_error_ptr(ping);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
      "  libpng-%s warning: %s", PNG_LIBPNG_VER_STRING,message);

  (void) ThrowMagickException(&image->exception,GetMagickModule(),CoderWarning,
    message,"`%s'",image->filename);
}