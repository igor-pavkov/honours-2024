MagickExport size_t CopyMagickString(char *destination,const char *source,
  const size_t length)
{
  register char
    *q;

  register const char
    *p;

  register size_t
    n;

  p=source;
  q=destination;
  for (n=length; n > 4; n-=4)
  {
    *q=(*p++);
    if (*q == '\0')
      return((size_t) (p-source-1));
    q++;
    *q=(*p++);
    if (*q == '\0')
      return((size_t) (p-source-1));
    q++;
    *q=(*p++);
    if (*q == '\0')
      return((size_t) (p-source-1));
    q++;
    *q=(*p++);
    if (*q == '\0')
      return((size_t) (p-source-1));
    q++;
  }
  if (n != 0)
    for (n--; n != 0; n--)
    {
      *q=(*p++);
      if (*q == '\0')
        return((size_t) (p-source-1));
      q++;
    }
  if (length != 0)
    *q='\0';
  return((size_t) (p-source-1));
}