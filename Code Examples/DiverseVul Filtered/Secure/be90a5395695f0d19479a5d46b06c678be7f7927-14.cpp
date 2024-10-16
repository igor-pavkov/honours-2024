MagickExport MagickBooleanType IsStringFalse(const char *value)
{
  if (value == (const char *) NULL)
    return(MagickFalse);
  if (LocaleCompare(value,"false") == 0)
    return(MagickTrue);
  if (LocaleCompare(value,"off") == 0)
    return(MagickTrue);
  if (LocaleCompare(value,"no") == 0)
    return(MagickTrue);
  if (LocaleCompare(value,"0") == 0)
    return(MagickTrue);
  return(MagickFalse);
}