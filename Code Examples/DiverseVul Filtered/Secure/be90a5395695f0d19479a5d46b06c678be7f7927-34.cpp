MagickExport char **StringToStrings(const char *text,size_t *count)
{
  char
    **textlist;

  register const char
    *p;

  register ssize_t
    i;

  size_t
    lines;

  if (text == (char *) NULL)
    {
      if (count != (size_t *) NULL)
        *count=0;
      return((char **) NULL);
    }
  for (p=text; *p != '\0'; p++)
    if (((int) ((unsigned char) *p) < 32) &&
        (isspace((int) ((unsigned char) *p)) == 0))
      break;
  if (*p == '\0')
    {
      register const char
        *q;

      /*
        Convert string to an ASCII list.
      */
      lines=1;
      for (p=text; *p != '\0'; p++)
        if (*p == '\n')
          lines++;
      textlist=(char **) AcquireQuantumMemory((size_t) lines+1UL,
        sizeof(*textlist));
      if (textlist == (char **) NULL)
        ThrowFatalException(ResourceLimitFatalError,"UnableToConvertText");
      p=text;
      for (i=0; i < (ssize_t) lines; i++)
      {
        for (q=p; *q != '\0'; q++)
          if ((*q == '\r') || (*q == '\n'))
            break;
        textlist[i]=(char *) AcquireQuantumMemory((size_t) (q-p)+1,
          sizeof(**textlist));
        if (textlist[i] == (char *) NULL)
          ThrowFatalException(ResourceLimitFatalError,"UnableToConvertText");
        (void) memcpy(textlist[i],p,(size_t) (q-p));
        textlist[i][q-p]='\0';
        if (*q == '\r')
          q++;
        p=q+1;
      }
    }
  else
    {
      char
        hex_string[MagickPathExtent];

      register char
        *q;

      register ssize_t
        j;

      /*
        Convert string to a HEX list.
      */
      lines=(size_t) (strlen(text)/CharsPerLine)+1;
      textlist=(char **) AcquireQuantumMemory((size_t) lines+1UL,
        sizeof(*textlist));
      if (textlist == (char **) NULL)
        ThrowFatalException(ResourceLimitFatalError,"UnableToConvertText");
      p=text;
      for (i=0; i < (ssize_t) lines; i++)
      {
        textlist[i]=(char *) AcquireQuantumMemory(2UL*MagickPathExtent,
          sizeof(**textlist));
        if (textlist[i] == (char *) NULL)
          ThrowFatalException(ResourceLimitFatalError,"UnableToConvertText");
        (void) FormatLocaleString(textlist[i],MagickPathExtent,"0x%08lx: ",
          (long) (CharsPerLine*i));
        q=textlist[i]+strlen(textlist[i]);
        for (j=1; j <= (ssize_t) MagickMin(strlen(p),CharsPerLine); j++)
        {
          (void) FormatLocaleString(hex_string,MagickPathExtent,"%02x",*(p+j));
          (void) CopyMagickString(q,hex_string,MagickPathExtent);
          q+=2;
          if ((j % 0x04) == 0)
            *q++=' ';
        }
        for ( ; j <= CharsPerLine; j++)
        {
          *q++=' ';
          *q++=' ';
          if ((j % 0x04) == 0)
            *q++=' ';
        }
        *q++=' ';
        for (j=1; j <= (ssize_t) MagickMin(strlen(p),CharsPerLine); j++)
        {
          if (isprint((int) ((unsigned char) *p)) != 0)
            *q++=(*p);
          else
            *q++='-';
          p++;
        }
        *q='\0';
        textlist[i]=(char *) ResizeQuantumMemory(textlist[i],q-textlist[i]+1,
          sizeof(**textlist));
        if (textlist[i] == (char *) NULL)
          ThrowFatalException(ResourceLimitFatalError,"UnableToConvertText");
      }
    }
  if (count != (size_t *) NULL)
    *count=lines;
  textlist[i]=(char *) NULL;
  return(textlist);
}