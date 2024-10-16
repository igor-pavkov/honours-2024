ModuleExport size_t RegisterDPXImage(void)
{
  MagickInfo
    *entry;

  static const char
    *DPXNote =
    {
      "Digital Moving Picture Exchange Bitmap, Version 2.0.\n"
      "See SMPTE 268M-2003 specification at http://www.smtpe.org\n"
    };

  entry=AcquireMagickInfo("DPX","DPX","SMPTE 268M-2003 (DPX 2.0)");
  entry->decoder=(DecodeImageHandler *) ReadDPXImage;
  entry->encoder=(EncodeImageHandler *) WriteDPXImage;
  entry->magick=(IsImageFormatHandler *) IsDPX;
  entry->flags^=CoderAdjoinFlag;
  entry->flags|=CoderDecoderSeekableStreamFlag;
  entry->note=ConstantString(DPXNote);
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}