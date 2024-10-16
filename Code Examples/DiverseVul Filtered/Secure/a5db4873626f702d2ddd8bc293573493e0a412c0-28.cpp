static void SVGCharacters(void *context,const xmlChar *c,int length)
{
  char
    *text;

  register char
    *p;

  register ssize_t
    i;

  SVGInfo
    *svg_info;

  /*
    Receiving some characters from the parser.
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
    "  SAX.characters(%s,%.20g)",c,(double) length);
  svg_info=(SVGInfo *) context;
  text=(char *) AcquireQuantumMemory(length+1,sizeof(*text));
  if (text == (char *) NULL)
    return;
  p=text;
  for (i=0; i < (ssize_t) length; i++)
    *p++=c[i];
  *p='\0';
  SVGStripString(MagickFalse,text);
  if (svg_info->text == (char *) NULL)
    svg_info->text=text;
  else
    {
      (void) ConcatenateString(&svg_info->text,text);
      text=DestroyString(text);
    }
}