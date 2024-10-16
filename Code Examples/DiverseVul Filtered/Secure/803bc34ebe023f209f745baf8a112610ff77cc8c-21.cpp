static void TIFFWarnings(const char *module,const char *format,va_list warning)
{
  char
    message[MagickPathExtent];

  ExceptionInfo
    *exception;

#if defined(MAGICKCORE_HAVE_VSNPRINTF)
  (void) vsnprintf(message,MagickPathExtent,format,warning);
#else
  (void) vsprintf(message,format,warning);
#endif
  (void) ConcatenateMagickString(message,".",MagickPathExtent);
  exception=(ExceptionInfo *) GetMagickThreadValue(tiff_exception);
  if (exception != (ExceptionInfo *) NULL)
    (void) ThrowMagickException(exception,GetMagickModule(),CoderWarning,
      message,"`%s'",module);
}