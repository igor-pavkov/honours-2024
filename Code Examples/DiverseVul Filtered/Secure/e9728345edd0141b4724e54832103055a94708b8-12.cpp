static MagickBooleanType IsDPX(const unsigned char *magick,const size_t extent)
{
  if (extent < 4)
    return(MagickFalse);
  if (memcmp(magick,"SDPX",4) == 0)
    return(MagickTrue);
  if (memcmp(magick,"XPDS",4) == 0)
    return(MagickTrue);
  return(MagickFalse);
}