MagickExport void PrintStringInfo(FILE *file,const char *id,
  const StringInfo *string_info)
{
  register const char
    *p;

  register size_t
    i,
    j;

  assert(id != (const char *) NULL);
  assert(string_info != (StringInfo *) NULL);
  assert(string_info->signature == MagickCoreSignature);
  p=(char *) string_info->datum;
  for (i=0; i < string_info->length; i++)
  {
    if (((int) ((unsigned char) *p) < 32) &&
        (isspace((int) ((unsigned char) *p)) == 0))
      break;
    p++;
  }
  (void) FormatLocaleFile(file,"%s(%.20g):\n",id,(double) string_info->length);
  if (i == string_info->length)
    {
      for (i=0; i < string_info->length; i++)
        (void) fputc(string_info->datum[i],file);
      (void) fputc('\n',file);
      return;
    }
  /*
    Convert string to a HEX list.
  */
  p=(char *) string_info->datum;
  for (i=0; i < string_info->length; i+=CharsPerLine)
  {
    (void) FormatLocaleFile(file,"0x%08lx: ",(unsigned long) (CharsPerLine*i));
    for (j=1; j <= MagickMin(string_info->length-i,CharsPerLine); j++)
    {
      (void) FormatLocaleFile(file,"%02lx",(unsigned long) (*(p+j)) & 0xff);
      if ((j % 0x04) == 0)
        (void) fputc(' ',file);
    }
    for ( ; j <= CharsPerLine; j++)
    {
      (void) fputc(' ',file);
      (void) fputc(' ',file);
      if ((j % 0x04) == 0)
        (void) fputc(' ',file);
    }
    (void) fputc(' ',file);
    for (j=1; j <= MagickMin(string_info->length-i,CharsPerLine); j++)
    {
      if (isprint((int) ((unsigned char) *p)) != 0)
        (void) fputc(*p,file);
      else
        (void) fputc('-',file);
      p++;
    }
    (void) fputc('\n',file);
  }
}