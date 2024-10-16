static void SVGReference(void *context,const xmlChar *name)
{
  SVGInfo
    *svg_info;

  xmlParserCtxtPtr
    parser;

  /*
    Called when an entity reference is detected.
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),"  SAX.reference(%s)",
    name);
  svg_info=(SVGInfo *) context;
  parser=svg_info->parser;
  if (parser == (xmlParserCtxtPtr) NULL)
    return;
  if (parser->node == (xmlNodePtr) NULL)
    return;
  if (*name == '#')
    (void) xmlAddChild(parser->node,xmlNewCharRef(svg_info->document,name));
  else
    (void) xmlAddChild(parser->node,xmlNewReference(svg_info->document,name));
}