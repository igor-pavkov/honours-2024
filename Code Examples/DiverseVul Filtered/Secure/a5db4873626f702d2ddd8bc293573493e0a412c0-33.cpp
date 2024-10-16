static xmlEntityPtr SVGGetParameterEntity(void *context,const xmlChar *name)
{
  SVGInfo
    *svg_info;

  /*
    Get a parameter entity by name.
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
    "  SAX.getParameterEntity(%s)",name);
  svg_info=(SVGInfo *) context;
  return(xmlGetParameterEntity(svg_info->document,name));
}