ModuleExport size_t RegisterMIFFImage(void)
{
  char
    version[MaxTextExtent];

  MagickInfo
    *entry;

  *version='\0';
#if defined(MagickImageCoderSignatureText)
  (void) CopyMagickString(version,MagickLibVersionText,MaxTextExtent);
#if defined(ZLIB_VERSION)
  (void) ConcatenateMagickString(version," with Zlib ",MaxTextExtent);
  (void) ConcatenateMagickString(version,ZLIB_VERSION,MaxTextExtent);
#endif
#if defined(MAGICKCORE_BZLIB_DELEGATE)
  (void) ConcatenateMagickString(version," and BZlib",MaxTextExtent);
#endif
#endif
  entry=SetMagickInfo("MIFF");
  entry->decoder=(DecodeImageHandler *) ReadMIFFImage;
  entry->encoder=(EncodeImageHandler *) WriteMIFFImage;
  entry->magick=(IsImageFormatHandler *) IsMIFF;
  entry->seekable_stream=MagickTrue;
  entry->description=ConstantString("Magick Image File Format");
  if (*version != '\0')
    entry->version=ConstantString(version);
  entry->module=ConstantString("MIFF");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}