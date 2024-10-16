MagickExport Image *LiquidRescaleImage(const Image *image,
  const size_t magick_unused(columns),const size_t magick_unused(rows),
  const double magick_unused(delta_x),const double magick_unused(rigidity),
  ExceptionInfo *exception)
{
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  (void) ThrowMagickException(exception,GetMagickModule(),MissingDelegateError,
    "DelegateLibrarySupportNotBuiltIn","'%s' (LQR)",image->filename);
  return((Image *) NULL);
}