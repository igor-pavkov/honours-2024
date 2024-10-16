ModuleExport void UnregisterPCDImage(void)
{
  (void) UnregisterMagickInfo("PCD");
  (void) UnregisterMagickInfo("PCDS");
}