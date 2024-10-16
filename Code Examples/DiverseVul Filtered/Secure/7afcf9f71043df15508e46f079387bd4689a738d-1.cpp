static MagickBooleanType IsSGI(const unsigned char *magick,const size_t length)
{
  if (length < 2)
    return(MagickFalse);
  if (memcmp(magick,"\001\332",2) == 0)
    return(MagickTrue);
  return(MagickFalse);
}