ModuleExport size_t RegisterXPMImage(void)
{
  MagickInfo
    *entry;

  if (xpm_symbolic == (SplayTreeInfo *) NULL)
    xpm_symbolic=NewSplayTree(CompareSplayTreeString,RelinquishMagickMemory,
      RelinquishMagickMemory);
  entry=AcquireMagickInfo("XPM","PICON","Personal Icon");
  entry->decoder=(DecodeImageHandler *) ReadXPMImage;
  entry->encoder=(EncodeImageHandler *) WritePICONImage;
  entry->flags^=CoderAdjoinFlag;
  (void) RegisterMagickInfo(entry);
  entry=AcquireMagickInfo("XPM","PM","X Windows system pixmap (color)");
  entry->decoder=(DecodeImageHandler *) ReadXPMImage;
  entry->encoder=(EncodeImageHandler *) WriteXPMImage;
  entry->flags^=CoderAdjoinFlag;
  entry->flags|=CoderStealthFlag;
  (void) RegisterMagickInfo(entry);
  entry=AcquireMagickInfo("XPM","XPM","X Windows system pixmap (color)");
  entry->decoder=(DecodeImageHandler *) ReadXPMImage;
  entry->encoder=(EncodeImageHandler *) WriteXPMImage;
  entry->magick=(IsImageFormatHandler *) IsXPM;
  entry->flags^=CoderAdjoinFlag;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}