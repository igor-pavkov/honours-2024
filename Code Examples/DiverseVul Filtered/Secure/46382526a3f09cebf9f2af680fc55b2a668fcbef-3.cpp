ModuleExport void UnregisterMETAImage(void)
{
  (void) UnregisterMagickInfo("8BIM");
  (void) UnregisterMagickInfo("8BIMTEXT");
  (void) UnregisterMagickInfo("8BIMWTEXT");
  (void) UnregisterMagickInfo("EXIF");
  (void) UnregisterMagickInfo("APP1");
  (void) UnregisterMagickInfo("APP1JPEG");
  (void) UnregisterMagickInfo("ICCTEXT");
  (void) UnregisterMagickInfo("ICM");
  (void) UnregisterMagickInfo("ICC");
  (void) UnregisterMagickInfo("IPTC");
  (void) UnregisterMagickInfo("IPTCTEXT");
  (void) UnregisterMagickInfo("IPTCWTEXT");
  (void) UnregisterMagickInfo("XMP");
}