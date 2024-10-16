static MagickBooleanType IsMETA(const unsigned char *magick,const size_t length)
{
  if (length < 4)
    return(MagickFalse);
  if (LocaleNCompare((char *) magick,"8BIM",4) == 0)
    return(MagickTrue);
  if (LocaleNCompare((char *) magick,"APP1",4) == 0)
    return(MagickTrue);
  if (LocaleNCompare((char *) magick,"\034\002",2) == 0)
    return(MagickTrue);
  return(MagickFalse);
}