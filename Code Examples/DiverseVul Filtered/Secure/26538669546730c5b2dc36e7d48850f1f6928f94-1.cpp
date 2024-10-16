ModuleExport void UnregisterXPMImage(void)
{
  (void) UnregisterMagickInfo("PICON");
  (void) UnregisterMagickInfo("PM");
  (void) UnregisterMagickInfo("XPM");
  if (xpm_symbolic != (SplayTreeInfo *) NULL)
    xpm_symbolic=DestroySplayTree(xpm_symbolic);
}