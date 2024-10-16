static MagickBooleanType IsXWD(const unsigned char *magick,const size_t length)
{
  if (length < 8)
    return(MagickFalse);
  if (memcmp(magick+1,"\000\000",2) == 0)
    {
      if (memcmp(magick+4,"\007\000\000",3) == 0)
        return(MagickTrue);
      if (memcmp(magick+5,"\000\000\007",3) == 0)
        return(MagickTrue);
    }
  return(MagickFalse);
}