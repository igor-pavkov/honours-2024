static MagickBooleanType IsMIFF(const unsigned char *magick,const size_t length)
{
  if (length < 14)
    return(MagickFalse);
  if (LocaleNCompare((const char *) magick,"id=ImageMagick",14) == 0)
    return(MagickTrue);
  return(MagickFalse);
}