ModuleExport size_t RegisterPDFImage(void)
{
  MagickInfo
    *entry;

  entry=AcquireMagickInfo("PDF","AI","Adobe Illustrator CS2");
  entry->decoder=(DecodeImageHandler *) ReadPDFImage;
  entry->encoder=(EncodeImageHandler *) WritePDFImage;
  entry->flags^=CoderAdjoinFlag;
  entry->flags^=CoderBlobSupportFlag;
  entry->mime_type=ConstantString("application/pdf");
  (void) RegisterMagickInfo(entry);
  entry=AcquireMagickInfo("PDF","EPDF",
    "Encapsulated Portable Document Format");
  entry->decoder=(DecodeImageHandler *) ReadPDFImage;
  entry->encoder=(EncodeImageHandler *) WritePDFImage;
  entry->flags^=CoderAdjoinFlag;
  entry->flags^=CoderBlobSupportFlag;
  entry->mime_type=ConstantString("application/pdf");
  (void) RegisterMagickInfo(entry);
  entry=AcquireMagickInfo("PDF","PDF","Portable Document Format");
  entry->decoder=(DecodeImageHandler *) ReadPDFImage;
  entry->encoder=(EncodeImageHandler *) WritePDFImage;
  entry->magick=(IsImageFormatHandler *) IsPDF;
  entry->flags^=CoderBlobSupportFlag;
  entry->mime_type=ConstantString("application/pdf");
  (void) RegisterMagickInfo(entry);
  entry=AcquireMagickInfo("PDF","PDFA","Portable Document Archive Format");
  entry->decoder=(DecodeImageHandler *) ReadPDFImage;
  entry->encoder=(EncodeImageHandler *) WritePDFImage;
  entry->magick=(IsImageFormatHandler *) IsPDF;
  entry->flags^=CoderBlobSupportFlag;
  entry->mime_type=ConstantString("application/pdf");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}