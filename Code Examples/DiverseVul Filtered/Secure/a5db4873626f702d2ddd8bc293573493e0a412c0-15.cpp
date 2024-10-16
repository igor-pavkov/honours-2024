static int SVGHasExternalSubset(void *context)
{
  SVGInfo
    *svg_info;

  /*
    Does this document has an external subset?
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
    "  SAX.SVGHasExternalSubset()");
  svg_info=(SVGInfo *) context;
  return(svg_info->document->extSubset != NULL);
}