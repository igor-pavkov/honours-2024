MagickExport MagickBooleanType IsPaletteImage(const Image *image)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  if (image->storage_class != PseudoClass)
    return(MagickFalse);
  return((image->colors <= 256) ? MagickTrue : MagickFalse);
}