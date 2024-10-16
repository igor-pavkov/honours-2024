static char **SVGKeyValuePairs(void *context,const int key_sentinel,
  const int value_sentinel,const char *text,size_t *number_tokens)
{
  char
    **tokens;

  register const char
    *p,
    *q;

  register ssize_t
    i;

  size_t
    extent;

  SVGInfo
    *svg_info;

  svg_info=(SVGInfo *) context;
  *number_tokens=0;
  if (text == (const char *) NULL)
    return((char **) NULL);
  extent=8;
  tokens=(char **) AcquireQuantumMemory(extent+2UL,sizeof(*tokens));
  if (tokens == (char **) NULL)
    {
      (void) ThrowMagickException(svg_info->exception,GetMagickModule(),
        ResourceLimitError,"MemoryAllocationFailed","`%s'",text);
      return((char **) NULL);
    }
  /*
    Convert string to an ASCII list.
  */
  i=0;
  p=text;
  for (q=p; *q != '\0'; q++)
  {
    if ((*q != key_sentinel) && (*q != value_sentinel) && (*q != '\0'))
      continue;
    if (i == (ssize_t) extent)
      {
        extent<<=1;
        tokens=(char **) ResizeQuantumMemory(tokens,extent+2,sizeof(*tokens));
        if (tokens == (char **) NULL)
          {
            (void) ThrowMagickException(svg_info->exception,GetMagickModule(),
              ResourceLimitError,"MemoryAllocationFailed","`%s'",text);
            return((char **) NULL);
          }
      }
    tokens[i]=AcquireString(p);
    (void) CopyMagickString(tokens[i],p,(size_t) (q-p+1));
    SVGStripString(MagickTrue,tokens[i]);
    i++;
    p=q+1;
  }
  tokens[i]=AcquireString(p);
  (void) CopyMagickString(tokens[i],p,(size_t) (q-p+1));
  SVGStripString(MagickTrue,tokens[i++]);
  tokens[i]=(char *) NULL;
  *number_tokens=(size_t) i;
  return(tokens);
}