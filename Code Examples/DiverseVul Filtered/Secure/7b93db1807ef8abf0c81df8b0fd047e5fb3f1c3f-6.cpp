ModuleExport size_t RegisterPDBImage(void)
{
  MagickInfo
    *entry;

  entry=AcquireMagickInfo("PDB","PDB","Palm Database ImageViewer Format");
  entry->decoder=(DecodeImageHandler *) ReadPDBImage;
  entry->encoder=(EncodeImageHandler *) WritePDBImage;
  entry->magick=(IsImageFormatHandler *) IsPDB;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}