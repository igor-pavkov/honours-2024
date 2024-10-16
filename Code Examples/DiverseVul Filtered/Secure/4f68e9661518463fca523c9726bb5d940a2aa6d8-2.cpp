ModuleExport size_t RegisterTGAImage(void)
{
  MagickInfo
    *entry;

  entry=AcquireMagickInfo("TGA","ICB","Truevision Targa image");
  entry->decoder=(DecodeImageHandler *) ReadTGAImage;
  entry->encoder=(EncodeImageHandler *) WriteTGAImage;
  entry->flags^=CoderAdjoinFlag;
  (void) RegisterMagickInfo(entry);
  entry=AcquireMagickInfo("TGA","TGA","Truevision Targa image");
  entry->decoder=(DecodeImageHandler *) ReadTGAImage;
  entry->encoder=(EncodeImageHandler *) WriteTGAImage;
  entry->flags^=CoderAdjoinFlag;
  (void) RegisterMagickInfo(entry);
  entry=AcquireMagickInfo("TGA","VDA","Truevision Targa image");
  entry->decoder=(DecodeImageHandler *) ReadTGAImage;
  entry->encoder=(EncodeImageHandler *) WriteTGAImage;
  entry->flags^=CoderAdjoinFlag;
  (void) RegisterMagickInfo(entry);
  entry=AcquireMagickInfo("TGA","VST","Truevision Targa image");
  entry->decoder=(DecodeImageHandler *) ReadTGAImage;
  entry->encoder=(EncodeImageHandler *) WriteTGAImage;
  entry->flags^=CoderAdjoinFlag;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}