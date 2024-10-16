static MagickBooleanType SGIDecode(const size_t bytes_per_pixel,
  ssize_t number_packets,unsigned char *packets,ssize_t number_pixels,
  unsigned char *pixels)
{
  register unsigned char
    *p,
    *q;

  size_t
    pixel;

  ssize_t
    count;

  p=packets;
  q=pixels;
  if (bytes_per_pixel == 2)
    {
      for ( ; number_pixels > 0; )
      {
        if (number_packets-- == 0)
          return(MagickFalse);
        pixel=(size_t) (*p++) << 8;
        pixel|=(*p++);
        count=(ssize_t) (pixel & 0x7f);
        if (count == 0)
          break;
        if (count > (ssize_t) number_pixels)
          return(MagickFalse);
        number_pixels-=count;
        if ((pixel & 0x80) != 0)
          for ( ; count != 0; count--)
          {
            if (number_packets-- == 0)
              return(MagickFalse);
            *q=(*p++);
            *(q+1)=(*p++);
            q+=8;
          }
        else
          {
            if (number_packets-- == 0)
              return(MagickFalse);
            pixel=(size_t) (*p++) << 8;
            pixel|=(*p++);
            for ( ; count != 0; count--)
            {
              *q=(unsigned char) (pixel >> 8);
              *(q+1)=(unsigned char) pixel;
              q+=8;
            }
          }
      }
      return(MagickTrue);
    }
  for ( ; number_pixels > 0; )
  {
    if (number_packets-- == 0)
      return(MagickFalse);
    pixel=(size_t) (*p++);
    count=(ssize_t) (pixel & 0x7f);
    if (count == 0)
      break;
    if (count > (ssize_t) number_pixels)
      return(MagickFalse);
    number_pixels-=count;
    if ((pixel & 0x80) != 0)
      for ( ; count != 0; count--)
      {
        if (number_packets-- == 0)
          return(MagickFalse);
        *q=(*p++);
        q+=4;
      }
    else
      {
        if (number_packets-- == 0)
          return(MagickFalse);
        pixel=(size_t) (*p++);
        for ( ; count != 0; count--)
        {
          *q=(unsigned char) pixel;
          q+=4;
        }
      }
  }
  return(MagickTrue);
}