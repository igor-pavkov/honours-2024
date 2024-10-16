ModuleExport size_t RegisterMATImage(void)
{
  MagickInfo
    *entry;

  entry=AcquireMagickInfo("MAT","MAT","MATLAB level 5 image format");
  entry->decoder=(DecodeImageHandler *) ReadMATImage;
  entry->encoder=(EncodeImageHandler *) WriteMATImage;
  entry->flags^=CoderBlobSupportFlag;
  entry->flags|=CoderDecoderSeekableStreamFlag;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}