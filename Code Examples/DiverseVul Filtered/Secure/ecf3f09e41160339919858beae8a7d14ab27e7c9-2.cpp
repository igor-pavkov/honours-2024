ModuleExport size_t RegisterVIFFImage(void)
{
  MagickInfo
    *entry;

  entry=AcquireMagickInfo("VIFF","VIFF","Khoros Visualization image");
  entry->decoder=(DecodeImageHandler *) ReadVIFFImage;
  entry->encoder=(EncodeImageHandler *) WriteVIFFImage;
  entry->magick=(IsImageFormatHandler *) IsVIFF;
  (void) RegisterMagickInfo(entry);
  entry=AcquireMagickInfo("VIFF","XV","Khoros Visualization image");
  entry->decoder=(DecodeImageHandler *) ReadVIFFImage;
  entry->encoder=(EncodeImageHandler *) WriteVIFFImage;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}