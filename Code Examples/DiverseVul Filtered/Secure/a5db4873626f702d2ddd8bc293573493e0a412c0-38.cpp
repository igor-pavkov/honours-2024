static MagickBooleanType IsSVG(const unsigned char *magick,const size_t length)
{
  if (length < 4)
    return(MagickFalse);
  if (LocaleNCompare((const char *) magick+1,"svg",3) == 0)
    return(MagickTrue);
  if (length < 5)
    return(MagickFalse);
  if (LocaleNCompare((const char *) magick+1,"?xml",4) == 0)
    return(MagickTrue);
  return(MagickFalse);
}