MagickExport MagickBooleanType IsHistogramImage(const Image *image,
  ExceptionInfo *exception)
{
#define MaximumUniqueColors  1024

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  return(CheckImageColors(image,exception,MaximumUniqueColors));
}