static MagickBooleanType IsJNG(const unsigned char *magick,const size_t length)
{
  if (length < 8)
    return(MagickFalse);

  if (memcmp(magick,"\213JNG\r\n\032\n",8) == 0)
    return(MagickTrue);

  return(MagickFalse);
}