MagickExport MagickBooleanType SetImageStorageClass(Image *image,
  const ClassType storage_class,ExceptionInfo *exception)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  image->storage_class=storage_class;
  return(SyncImagePixelCache(image,exception));
}