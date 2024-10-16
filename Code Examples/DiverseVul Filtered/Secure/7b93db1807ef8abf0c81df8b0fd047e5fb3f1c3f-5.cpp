static MagickBooleanType IsPDB(const unsigned char *magick,const size_t length)
{
  if (length < 68)
    return(MagickFalse);
  if (memcmp(magick+60,"vIMGView",8) == 0)
    return(MagickTrue);
  return(MagickFalse);
}