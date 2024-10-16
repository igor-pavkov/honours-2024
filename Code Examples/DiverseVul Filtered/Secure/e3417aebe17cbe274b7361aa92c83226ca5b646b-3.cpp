ModuleExport void UnregisterSVGImage(void)
{
  (void) UnregisterMagickInfo("SVGZ");
  (void) UnregisterMagickInfo("SVG");
  (void) UnregisterMagickInfo("MSVG");
}