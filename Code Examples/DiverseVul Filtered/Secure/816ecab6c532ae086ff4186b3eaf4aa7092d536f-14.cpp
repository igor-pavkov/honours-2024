static MagickBooleanType IsPNG(const unsigned char *magick,const size_t length)
{
  if (length < 8)
    return(MagickFalse);

  if (memcmp(magick,"\211PNG\r\n\032\n",8) == 0)
    return(MagickTrue);

  return(MagickFalse);
}