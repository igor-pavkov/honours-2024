static void SVGComment(void *context,const xmlChar *value)
{
  SVGInfo
    *svg_info;

  /*
    A comment has been parsed.
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),"  SAX.comment(%s)",
    value);
  svg_info=(SVGInfo *) context;
  if (svg_info->comment != (char *) NULL)
    (void) ConcatenateString(&svg_info->comment,"\n");
  (void) ConcatenateString(&svg_info->comment,(const char *) value);
}