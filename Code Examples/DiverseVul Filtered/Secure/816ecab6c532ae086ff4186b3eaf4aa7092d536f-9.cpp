static void MagickPNGWarningHandler(png_struct *ping,png_const_charp message)
{
  ExceptionInfo
    *exception;

  Image
    *image;

  PNGErrorInfo
    *error_info;

  if (LocaleCompare(message, "Missing PLTE before tRNS") == 0)
    png_error(ping, message);

  error_info=(PNGErrorInfo *) png_get_error_ptr(ping);
  image=error_info->image;
  exception=error_info->exception;
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
    "  libpng-%s warning: %s", png_get_libpng_ver(NULL),message);

  (void) ThrowMagickException(exception,GetMagickModule(),CoderWarning,
    message,"`%s'",image->filename);
}