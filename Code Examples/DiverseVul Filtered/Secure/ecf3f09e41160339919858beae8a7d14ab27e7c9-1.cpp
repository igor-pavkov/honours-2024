static MagickBooleanType IsVIFF(const unsigned char *magick,const size_t length)
{
  if (length < 2)
    return(MagickFalse);
  if (memcmp(magick,"\253\001",2) == 0)
    return(MagickTrue);
  return(MagickFalse);
}