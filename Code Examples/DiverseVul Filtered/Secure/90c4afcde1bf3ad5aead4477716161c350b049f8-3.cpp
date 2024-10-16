ModuleExport size_t RegisterJP2Image(void)
{
  char
    version[MagickPathExtent];

  MagickInfo
    *entry;

  *version='\0';
#if defined(MAGICKCORE_LIBOPENJP2_DELEGATE)
  (void) FormatLocaleString(version,MagickPathExtent,"%s",opj_version());
#endif
  entry=AcquireMagickInfo("JP2","JP2","JPEG-2000 File Format Syntax");
  if (*version != '\0')
    entry->version=ConstantString(version);
  entry->mime_type=ConstantString("image/jp2");
  entry->magick=(IsImageFormatHandler *) IsJP2;
  entry->flags^=CoderAdjoinFlag;
  entry->flags|=CoderDecoderSeekableStreamFlag;
  entry->flags|=CoderEncoderSeekableStreamFlag;
#if defined(MAGICKCORE_LIBOPENJP2_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadJP2Image;
  entry->encoder=(EncodeImageHandler *) WriteJP2Image;
#endif
  (void) RegisterMagickInfo(entry);
  entry=AcquireMagickInfo("JP2","J2C","JPEG-2000 Code Stream Syntax");
  if (*version != '\0')
    entry->version=ConstantString(version);
  entry->mime_type=ConstantString("image/jp2");
  entry->magick=(IsImageFormatHandler *) IsJ2K;
  entry->flags^=CoderAdjoinFlag;
  entry->flags|=CoderDecoderSeekableStreamFlag;
  entry->flags|=CoderEncoderSeekableStreamFlag;
#if defined(MAGICKCORE_LIBOPENJP2_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadJP2Image;
  entry->encoder=(EncodeImageHandler *) WriteJP2Image;
#endif
  (void) RegisterMagickInfo(entry);
  entry=AcquireMagickInfo("JP2","J2K","JPEG-2000 Code Stream Syntax");
  if (*version != '\0')
    entry->version=ConstantString(version);
  entry->mime_type=ConstantString("image/jp2");
  entry->magick=(IsImageFormatHandler *) IsJ2K;
  entry->flags^=CoderAdjoinFlag;
  entry->flags|=CoderDecoderSeekableStreamFlag;
  entry->flags|=CoderEncoderSeekableStreamFlag;
#if defined(MAGICKCORE_LIBOPENJP2_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadJP2Image;
  entry->encoder=(EncodeImageHandler *) WriteJP2Image;
#endif
  (void) RegisterMagickInfo(entry);
  entry=AcquireMagickInfo("JP2","JPM","JPEG-2000 File Format Syntax");
  if (*version != '\0')
    entry->version=ConstantString(version);
  entry->mime_type=ConstantString("image/jp2");
  entry->magick=(IsImageFormatHandler *) IsJP2;
  entry->flags^=CoderAdjoinFlag;
  entry->flags|=CoderDecoderSeekableStreamFlag;
  entry->flags|=CoderEncoderSeekableStreamFlag;
#if defined(MAGICKCORE_LIBOPENJP2_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadJP2Image;
  entry->encoder=(EncodeImageHandler *) WriteJP2Image;
#endif
  (void) RegisterMagickInfo(entry);
  entry=AcquireMagickInfo("JP2","JPT","JPEG-2000 File Format Syntax");
  if (*version != '\0')
    entry->version=ConstantString(version);
  entry->mime_type=ConstantString("image/jp2");
  entry->magick=(IsImageFormatHandler *) IsJP2;
  entry->flags^=CoderAdjoinFlag;
  entry->flags|=CoderDecoderSeekableStreamFlag;
  entry->flags|=CoderEncoderSeekableStreamFlag;
#if defined(MAGICKCORE_LIBOPENJP2_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadJP2Image;
  entry->encoder=(EncodeImageHandler *) WriteJP2Image;
#endif
  (void) RegisterMagickInfo(entry);
  entry=AcquireMagickInfo("JP2","JPC","JPEG-2000 Code Stream Syntax");
  if (*version != '\0')
    entry->version=ConstantString(version);
  entry->mime_type=ConstantString("image/jp2");
  entry->magick=(IsImageFormatHandler *) IsJP2;
  entry->flags^=CoderAdjoinFlag;
  entry->flags|=CoderDecoderSeekableStreamFlag;
  entry->flags|=CoderEncoderSeekableStreamFlag;
#if defined(MAGICKCORE_LIBOPENJP2_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadJP2Image;
  entry->encoder=(EncodeImageHandler *) WriteJP2Image;
#endif
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}