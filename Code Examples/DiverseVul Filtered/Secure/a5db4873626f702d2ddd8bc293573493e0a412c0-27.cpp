static void SVGInternalSubset(void *context,const xmlChar *name,
  const xmlChar *external_id,const xmlChar *system_id)
{
  SVGInfo
    *svg_info;

  /*
    Does this document has an internal subset?
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
    "  SAX.internalSubset(%s, %s, %s)",(const char *) name,
    (external_id != (const xmlChar *) NULL ? (const char *) external_id : "none"),
    (system_id != (const xmlChar *) NULL ? (const char *) system_id : "none"));
  svg_info=(SVGInfo *) context;
  (void) xmlCreateIntSubset(svg_info->document,name,external_id,system_id);
}