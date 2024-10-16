static void SVGIgnorableWhitespace(void *context,const xmlChar *c,int length)
{
  SVGInfo
    *svg_info;

  /*
    Receiving some ignorable whitespaces from the parser.
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
    "  SAX.ignorableWhitespace(%.30s, %d)",c,length);
  svg_info=(SVGInfo *) context;
  (void) svg_info;
}