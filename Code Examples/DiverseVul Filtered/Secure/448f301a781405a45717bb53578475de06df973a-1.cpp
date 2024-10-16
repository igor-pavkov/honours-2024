ModuleExport size_t RegisterPS2Image(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("EPS2");
  entry->encoder=(EncodeImageHandler *) WritePS2Image;
  entry->adjoin=MagickFalse;
  entry->seekable_stream=MagickTrue;
  entry->blob_support=MagickFalse;
  entry->description=ConstantString("Level II Encapsulated PostScript");
  entry->mime_type=ConstantString("application/postscript");
  entry->module=ConstantString("PS2");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("PS2");
  entry->encoder=(EncodeImageHandler *) WritePS2Image;
  entry->seekable_stream=MagickTrue;
  entry->blob_support=MagickFalse;
  entry->description=ConstantString("Level II PostScript");
  entry->mime_type=ConstantString("application/postscript");
  entry->module=ConstantString("PS2");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}