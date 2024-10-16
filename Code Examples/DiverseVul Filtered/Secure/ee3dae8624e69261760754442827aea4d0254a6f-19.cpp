MagickExport size_t InterpretImageFilename(const ImageInfo *image_info,
  Image *image,const char *format,int value,char *filename,
  ExceptionInfo *exception)
{
  char
    *q;

  int
    c;

  MagickBooleanType
    canonical;

  register const char
    *p;

  ssize_t
    field_width,
    offset;

  canonical=MagickFalse;
  offset=0;
  (void) CopyMagickString(filename,format,MagickPathExtent);
  for (p=strchr(format,'%'); p != (char *) NULL; p=strchr(p+1,'%'))
  {
    q=(char *) p+1;
    if (*q == '%')
      {
        p=q+1;
        continue;
      }
    field_width=0;
    if (*q == '0')
      field_width=(ssize_t) strtol(q,&q,10);
    switch (*q)
    {
      case 'd':
      case 'o':
      case 'x':
      {
        q++;
        c=(*q);
        *q='\0';
        (void) FormatLocaleString(filename+(p-format-offset),(size_t)
          (MagickPathExtent-(p-format-offset)),p,value);
        offset+=(4-field_width);
        *q=c;
        (void) ConcatenateMagickString(filename,q,MagickPathExtent);
        canonical=MagickTrue;
        if (*(q-1) != '%')
          break;
        p++;
        break;
      }
      case '[':
      {
        char
          pattern[MagickPathExtent];

        const char
          *option;

        register char
          *r;

        register ssize_t
          i;

        ssize_t
          depth;

        /*
          Image option.
        */
        if (strchr(p,']') == (char *) NULL)
          break;
        depth=1;
        r=q+1;
        for (i=0; (i < (MagickPathExtent-1L)) && (*r != '\0'); i++)
        {
          if (*r == '[')
            depth++;
          if (*r == ']')
            depth--;
          if (depth <= 0)
            break;
          pattern[i]=(*r++);
        }
        pattern[i]='\0';
        if (LocaleNCompare(pattern,"filename:",9) != 0)
          break;
        option=(const char *) NULL;
        if (image != (Image *) NULL)
          option=GetImageProperty(image,pattern,exception);
        if ((option == (const char *) NULL) && (image != (Image *) NULL))
          option=GetImageArtifact(image,pattern);
        if ((option == (const char *) NULL) &&
            (image_info != (ImageInfo *) NULL))
          option=GetImageOption(image_info,pattern);
        if (option == (const char *) NULL)
          break;
        q--;
        c=(*q);
        *q='\0';
        (void) CopyMagickString(filename+(p-format-offset),option,(size_t)
          (MagickPathExtent-(p-format-offset)));
        offset+=strlen(pattern)-strlen(option)+3;
        *q=c;
        (void) ConcatenateMagickString(filename,r+1,MagickPathExtent);
        canonical=MagickTrue;
        if (*(q-1) != '%')
          break;
        p++;
        break;
      }
      default:
        break;
    }
  }
  for (q=filename; *q != '\0'; q++)
    if ((*q == '%') && (*(q+1) == '%'))
      {
        (void) CopyMagickString(q,q+1,(size_t) (MagickPathExtent-(q-filename)));
        canonical=MagickTrue;
      }
  if (canonical == MagickFalse)
    (void) CopyMagickString(filename,format,MagickPathExtent);
  return(strlen(filename));
}