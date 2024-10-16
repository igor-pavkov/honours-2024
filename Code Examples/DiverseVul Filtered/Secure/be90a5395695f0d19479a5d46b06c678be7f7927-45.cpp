MagickExport char *StringToken(const char *delimiters,char **string)
{
  char
    *q;

  register char
    *p;

  register const char
    *r;

  register int
    c,
    d;

  p=(*string);
  if (p == (char *) NULL)
    return((char *) NULL);
  q=p;
  for ( ; ; )
  {
    c=(*p++);
    r=delimiters;
    do
    {
      d=(*r++);
      if (c == d)
        {
          if (c == '\0')
            p=(char *) NULL;
          else
            p[-1]='\0';
          *string=p;
          return(q);
        }
    } while (d != '\0');
  }
}