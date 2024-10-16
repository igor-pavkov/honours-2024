MagickExport MagickBooleanType GetImageAlphaChannel(const Image *image)
{
  assert(image != (const Image *) NULL);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(image->signature == MagickCoreSignature);
  return(image->alpha_trait != UndefinedPixelTrait ? MagickTrue : MagickFalse);
}