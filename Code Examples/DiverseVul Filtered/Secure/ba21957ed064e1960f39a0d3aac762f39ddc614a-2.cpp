ModuleExport size_t RegisterDOTImage(void)
{
  MagickInfo
    *entry;

  entry=AcquireMagickInfo("DOT","DOT","Graphviz");
#if defined(MAGICKCORE_GVC_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadDOTImage;
#endif
  entry->flags^=CoderBlobSupportFlag;
  (void) RegisterMagickInfo(entry);
  entry=AcquireMagickInfo("DOT","GV","Graphviz");
#if defined(MAGICKCORE_GVC_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadDOTImage;
#endif
  entry->flags^=CoderBlobSupportFlag;
  (void) RegisterMagickInfo(entry);
#if defined(MAGICKCORE_GVC_DELEGATE)
  graphic_context=gvContext();
#endif
  return(MagickImageCoderSignature);
}