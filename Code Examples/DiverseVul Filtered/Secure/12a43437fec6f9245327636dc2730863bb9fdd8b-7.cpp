static MagickBooleanType IsPCD(const unsigned char *magick,const size_t length)
{
  if (length < 2052)
    return(MagickFalse);
  if (LocaleNCompare((const char *) magick+2048,"PCD_",4) == 0)
    return(MagickTrue);
  return(MagickFalse);
}