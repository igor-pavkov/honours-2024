MagickExport size_t ConcatenateMagickString(char *destination,
  const char *source,const size_t length)
{
  register char
    *q;

  register const char
    *p;

  register size_t
    i;

  size_t
    count;

  assert(destination != (char *) NULL);
  assert(source != (const char *) NULL);
  assert(length >= 1);
  p=source;
  q=destination;
  i=length;
  while ((i-- != 0) && (*q != '\0'))
    q++;
  count=(size_t) (q-destination);
  i=length-count;
  if (i == 0)
    return(count+strlen(p));
  while (*p != '\0')
  {
    if (i != 1)
      {
        *q++=(*p);
        i--;
      }
    p++;
  }
  *q='\0';
  return(count+(p-source));
}