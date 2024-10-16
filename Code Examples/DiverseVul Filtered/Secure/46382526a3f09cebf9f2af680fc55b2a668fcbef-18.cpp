static char *super_fgets(char **b, int *blen, Image *file)
{
  int
    c,
    len;

  unsigned char
    *p,
    *q;

  len=*blen;
  p=(unsigned char *) (*b);
  for (q=p; ; q++)
  {
    c=ReadBlobByte(file);
    if (c == EOF || c == '\n')
      break;
    if ((q-p+1) >= (int) len)
      {
        int
          tlen;

        tlen=q-p;
        len<<=1;
        p=(unsigned char *) ResizeQuantumMemory(p,(size_t) len+2UL,sizeof(*p));
        *b=(char *) p;
        if (p == (unsigned char *) NULL)
          break;
        q=p+tlen;
      }
    *q=(unsigned char) c;
  }
  *blen=0;
  if (p != (unsigned char *) NULL)
    {
      int
        tlen;

      tlen=q-p;
      if (tlen == 0)
        return (char *) NULL;
      p[tlen] = '\0';
      *blen=++tlen;
    }
  return((char *) p);
}