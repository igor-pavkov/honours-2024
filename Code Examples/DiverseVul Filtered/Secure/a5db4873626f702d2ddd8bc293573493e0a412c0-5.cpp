static void SVGSetDocumentLocator(void *context,xmlSAXLocatorPtr location)
{
  SVGInfo
    *svg_info;

  /*
    Receive the document locator at startup, actually xmlDefaultSAXLocator.
  */
  (void) location;
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
    "  SAX.setDocumentLocator()");
  svg_info=(SVGInfo *) context;
  (void) svg_info;
}