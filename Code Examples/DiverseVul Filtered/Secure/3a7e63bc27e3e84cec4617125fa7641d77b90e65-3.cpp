ModuleExport size_t RegisterPALMImage(void)
{
  MagickInfo
    *entry;

  entry=AcquireMagickInfo("PALM","PALM","Palm pixmap");
  entry->decoder=(DecodeImageHandler *) ReadPALMImage;
  entry->encoder=(EncodeImageHandler *) WritePALMImage;
  entry->flags|=CoderDecoderSeekableStreamFlag;
  entry->flags|=CoderEncoderSeekableStreamFlag;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}