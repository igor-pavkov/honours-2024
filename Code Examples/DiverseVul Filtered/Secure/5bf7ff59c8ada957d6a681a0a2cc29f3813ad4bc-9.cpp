MagickExport MagickBooleanType FormatImageProperty(Image *image,
  const char *property,const char *format,...)
{
  char
    value[MaxTextExtent];

  ssize_t
    n;

  va_list
    operands;

  va_start(operands,format);
  n=FormatLocaleStringList(value,MaxTextExtent,format,operands);
  (void) n;
  va_end(operands);
  return(SetImageProperty(image,property,value));
}