ModuleExport void UnregisterVIFFImage(void)
{
  (void) UnregisterMagickInfo("VIFF");
  (void) UnregisterMagickInfo("XV");
}