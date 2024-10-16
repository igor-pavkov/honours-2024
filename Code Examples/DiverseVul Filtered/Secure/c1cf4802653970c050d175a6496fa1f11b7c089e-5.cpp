ModuleExport size_t RegisterPNMImage(void)
{
  MagickInfo
    *entry;

  entry=AcquireMagickInfo("PNM","PAM","Common 2-dimensional bitmap format");
  entry->decoder=(DecodeImageHandler *) ReadPNMImage;
  entry->encoder=(EncodeImageHandler *) WritePNMImage;
  entry->mime_type=ConstantString("image/x-portable-pixmap");
  (void) RegisterMagickInfo(entry);
  entry=AcquireMagickInfo("PNM","PBM",
    "Portable bitmap format (black and white)");
  entry->decoder=(DecodeImageHandler *) ReadPNMImage;
  entry->encoder=(EncodeImageHandler *) WritePNMImage;
  entry->mime_type=ConstantString("image/x-portable-bitmap");
  (void) RegisterMagickInfo(entry);
  entry=AcquireMagickInfo("PNM","PFM","Portable float format");
  entry->decoder=(DecodeImageHandler *) ReadPNMImage;
  entry->encoder=(EncodeImageHandler *) WritePNMImage;
  entry->flags|=CoderEndianSupportFlag;
  (void) RegisterMagickInfo(entry);
  entry=AcquireMagickInfo("PNM","PGM","Portable graymap format (gray scale)");
  entry->decoder=(DecodeImageHandler *) ReadPNMImage;
  entry->encoder=(EncodeImageHandler *) WritePNMImage;
  entry->mime_type=ConstantString("image/x-portable-greymap");
  (void) RegisterMagickInfo(entry);
  entry=AcquireMagickInfo("PNM","PNM","Portable anymap");
  entry->decoder=(DecodeImageHandler *) ReadPNMImage;
  entry->encoder=(EncodeImageHandler *) WritePNMImage;
  entry->magick=(IsImageFormatHandler *) IsPNM;
  entry->mime_type=ConstantString("image/x-portable-pixmap");
  (void) RegisterMagickInfo(entry);
  entry=AcquireMagickInfo("PNM","PPM","Portable pixmap format (color)");
  entry->decoder=(DecodeImageHandler *) ReadPNMImage;
  entry->encoder=(EncodeImageHandler *) WritePNMImage;
  entry->mime_type=ConstantString("image/x-portable-pixmap");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}