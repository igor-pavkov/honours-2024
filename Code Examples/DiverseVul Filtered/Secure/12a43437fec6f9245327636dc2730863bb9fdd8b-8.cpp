ModuleExport size_t RegisterPCDImage(void)
{
  MagickInfo
    *entry;

  entry=AcquireMagickInfo("PCD","PCD","Photo CD");
  entry->decoder=(DecodeImageHandler *) ReadPCDImage;
  entry->encoder=(EncodeImageHandler *) WritePCDImage;
  entry->magick=(IsImageFormatHandler *) IsPCD;
  entry->flags^=CoderAdjoinFlag;
  entry->flags|=CoderDecoderSeekableStreamFlag;
  (void) RegisterMagickInfo(entry);
  entry=AcquireMagickInfo("PCD","PCDS","Photo CD");
  entry->decoder=(DecodeImageHandler *) ReadPCDImage;
  entry->encoder=(EncodeImageHandler *) WritePCDImage;
  entry->flags^=CoderAdjoinFlag;
  entry->flags|=CoderDecoderSeekableStreamFlag;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}