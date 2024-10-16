ModuleExport size_t RegisterCIPImage(void)
{
  MagickInfo
    *entry;

  entry=AcquireMagickInfo("CIP","CIP","Cisco IP phone image format");
  entry->encoder=(EncodeImageHandler *) WriteCIPImage;
  entry->flags^=CoderAdjoinFlag;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}