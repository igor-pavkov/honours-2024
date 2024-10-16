static void SVGError(void *context,const char *format,...)
{
  char
    *message,
    reason[MaxTextExtent];

  SVGInfo
    *svg_info;

  va_list
    operands;

  /*
    Display and format a error formats, gives file, line, position and
    extra parameters.
  */
  va_start(operands,format);
  svg_info=(SVGInfo *) context;
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),"  SAX.error: ");
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),format,operands);
#if !defined(MAGICKCORE_HAVE_VSNPRINTF)
  (void) vsprintf(reason,format,operands);
#else
  (void) vsnprintf(reason,MaxTextExtent,format,operands);
#endif
  message=GetExceptionMessage(errno);
  (void) ThrowMagickException(svg_info->exception,GetMagickModule(),CoderError,
    reason,"`%s`",message);
  message=DestroyString(message);
  va_end(operands);
}