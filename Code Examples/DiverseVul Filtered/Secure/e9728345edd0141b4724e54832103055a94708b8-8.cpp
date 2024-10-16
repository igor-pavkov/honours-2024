static void TimeCodeToString(const size_t timestamp,char *code)
{
#define TimeFields  7

  unsigned int
    shift;

  register ssize_t
    i;

  *code='\0';
  shift=4*TimeFields;
  for (i=0; i <= TimeFields; i++)
  {
    (void) FormatLocaleString(code,MagickPathExtent-strlen(code),"%x",
      (unsigned int) ((timestamp >> shift) & 0x0fU));
    code++;
    if (((i % 2) != 0) && (i < TimeFields))
      *code++=':';
    shift-=4;
    *code='\0';
  }
}