ModuleExport void UnregisterDOTImage(void)
{
  (void) UnregisterMagickInfo("GV");
  (void) UnregisterMagickInfo("DOT");
#if defined(MAGICKCORE_GVC_DELEGATE)
  if (graphic_context != (GVC_t *) NULL)
    {
      gvFreeContext(graphic_context);
      graphic_context=(GVC_t *) NULL;
    }
#endif
}