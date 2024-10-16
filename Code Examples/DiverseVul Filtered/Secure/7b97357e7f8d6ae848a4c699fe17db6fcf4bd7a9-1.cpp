static MagickBooleanType IsXPM(const unsigned char *magick,const size_t length)
{
  if (length < 9)
    return(MagickFalse);
  if (LocaleNCompare((char *) magick+1,"* XPM *",7) == 0)
    return(MagickTrue);
  return(MagickFalse);
}