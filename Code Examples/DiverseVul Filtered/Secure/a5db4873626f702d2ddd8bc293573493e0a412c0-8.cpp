static void SVGWarning(void *context,const char *format,...)
{
  char
    *message,
    reason[MaxTextExtent];

  SVGInfo
    *svg_info;

  va_list
    operands;

  /**
    Display and format a warning messages, gives file, line, position and
    extra parameters.
  */
  va_start(operands,format);
  svg_info=(SVGInfo *) context;
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),"  SAX.warning: ");
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),format,operands);
#if !defined(MAGICKCORE_HAVE_VSNPRINTF)
  (void) vsprintf(reason,format,operands);
#else
  (void) vsnprintf(reason,MaxTextExtent,format,operands);
#endif
  message=GetExceptionMessage(errno);
  (void) ThrowMagickException(svg_info->exception,GetMagickModule(),
    DelegateWarning,reason,"`%s`",message);
  message=DestroyString(message);
  va_end(operands);
}