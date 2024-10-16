ModuleExport size_t RegisterPSDImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("PSB");
  entry->decoder=(DecodeImageHandler *) ReadPSDImage;
  entry->encoder=(EncodeImageHandler *) WritePSDImage;
  entry->magick=(IsImageFormatHandler *) IsPSD;
  entry->seekable_stream=MagickTrue;
  entry->description=ConstantString("Adobe Large Document Format");
  entry->module=ConstantString("PSD");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("PSD");
  entry->decoder=(DecodeImageHandler *) ReadPSDImage;
  entry->encoder=(EncodeImageHandler *) WritePSDImage;
  entry->magick=(IsImageFormatHandler *) IsPSD;
  entry->seekable_stream=MagickTrue;
  entry->description=ConstantString("Adobe Photoshop bitmap");
  entry->module=ConstantString("PSD");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}