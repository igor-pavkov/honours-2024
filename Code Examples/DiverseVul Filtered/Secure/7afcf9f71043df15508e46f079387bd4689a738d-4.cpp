static size_t SGIEncode(unsigned char *pixels,size_t length,
  unsigned char *packets)
{
  short
    runlength;

  register unsigned char
    *p,
    *q;

  unsigned char
    *limit,
    *mark;

  p=pixels;
  limit=p+length*4;
  q=packets;
  while (p < limit)
  {
    mark=p;
    p+=8;
    while ((p < limit) && ((*(p-8) != *(p-4)) || (*(p-4) != *p)))
      p+=4;
    p-=8;
    length=(size_t) (p-mark) >> 2;
    while (length != 0)
    {
      runlength=(short) (length > 126 ? 126 : length);
      length-=runlength;
      *q++=(unsigned char) (0x80 | runlength);
      for ( ; runlength > 0; runlength--)
      {
        *q++=(*mark);
        mark+=4;
      }
    }
    mark=p;
    p+=4;
    while ((p < limit) && (*p == *mark))
      p+=4;
    length=(size_t) (p-mark) >> 2;
    while (length != 0)
    {
      runlength=(short) (length > 126 ? 126 : length);
      length-=runlength;
      *q++=(unsigned char) runlength;
      *q++=(*mark);
    }
  }
  *q++='\0';
  return((size_t) (q-packets));
}