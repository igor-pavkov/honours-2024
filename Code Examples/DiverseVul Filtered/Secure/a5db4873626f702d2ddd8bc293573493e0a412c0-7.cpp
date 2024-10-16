static int SVGIsStandalone(void *context)
{
  SVGInfo
    *svg_info;

  /*
    Is this document tagged standalone?
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),"  SAX.SVGIsStandalone()");
  svg_info=(SVGInfo *) context;
  return(svg_info->document->standalone == 1);
}