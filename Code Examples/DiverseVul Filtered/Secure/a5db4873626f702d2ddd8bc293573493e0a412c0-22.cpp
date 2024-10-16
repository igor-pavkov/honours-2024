ModuleExport void UnregisterSVGImage(void)
{
  (void) UnregisterMagickInfo("SVGZ");
  (void) UnregisterMagickInfo("SVG");
  (void) UnregisterMagickInfo("MSVG");
#if defined(MAGICKCORE_XML_DELEGATE)
  xmlCleanupParser();
#endif
}