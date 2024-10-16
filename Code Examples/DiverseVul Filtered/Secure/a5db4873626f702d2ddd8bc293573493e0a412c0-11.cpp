static void SVGUnparsedEntityDeclaration(void *context,const xmlChar *name,
  const xmlChar *public_id,const xmlChar *system_id,const xmlChar *notation)
{
  SVGInfo
    *svg_info;

  /*
    What to do when an unparsed entity declaration is parsed.
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
    "  SAX.unparsedEntityDecl(%s, %s, %s, %s)",name,
    public_id != (xmlChar *) NULL ? (const char *) public_id : "none",
    system_id != (xmlChar *) NULL ? (const char *) system_id : "none",notation);
  svg_info=(SVGInfo *) context;
  (void) xmlAddDocEntity(svg_info->document,name,
    XML_EXTERNAL_GENERAL_UNPARSED_ENTITY,public_id,system_id,notation);

}