ModuleExport size_t RegisterCUTImage(void)
{
  MagickInfo
    *entry;

  entry=AcquireMagickInfo("CUT","CUT","DR Halo");
  entry->decoder=(DecodeImageHandler *) ReadCUTImage;
  entry->flags|=CoderDecoderSeekableStreamFlag;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}