ModuleExport size_t RegisterXCFImage(void)
{
  MagickInfo
    *entry;

  entry=AcquireMagickInfo("XCF","XCF","GIMP image");
  entry->decoder=(DecodeImageHandler *) ReadXCFImage;
  entry->magick=(IsImageFormatHandler *) IsXCF;
  entry->flags|=CoderSeekableStreamFlag;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}