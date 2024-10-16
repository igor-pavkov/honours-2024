MagickExport MagickBooleanType SyncImage(Image *image,ExceptionInfo *exception)
{
  CacheView
    *image_view;

  MagickBooleanType
    range_exception,
    status,
    taint;

  ssize_t
    y;

  assert(image != (Image *) NULL);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(image->signature == MagickCoreSignature);
  if (image->ping != MagickFalse)
    return(MagickTrue);
  if (image->storage_class != PseudoClass)
    return(MagickFalse);
  assert(image->colormap != (PixelInfo *) NULL);
  range_exception=MagickFalse;
  status=MagickTrue;
  taint=image->taint;
  image_view=AcquireAuthenticCacheView(image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(range_exception,status) \
    magick_number_threads(image,image,image->rows,1)
#endif
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    Quantum
      index;

    register Quantum
      *magick_restrict q;

    register ssize_t
      x;

    if (status == MagickFalse)
      continue;
    q=GetCacheViewAuthenticPixels(image_view,0,y,image->columns,1,exception);
    if (q == (Quantum *) NULL)
      {
        status=MagickFalse;
        continue;
      }
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      index=PushColormapIndex(image,GetPixelIndex(image,q),&range_exception);
      SetPixelViaPixelInfo(image,image->colormap+(ssize_t) index,q);
      q+=GetPixelChannels(image);
    }
    if (SyncCacheViewAuthenticPixels(image_view,exception) == MagickFalse)
      status=MagickFalse;
  }
  image_view=DestroyCacheView(image_view);
  image->taint=taint;
  if ((image->ping == MagickFalse) && (range_exception != MagickFalse))
    (void) ThrowMagickException(exception,GetMagickModule(),
      CorruptImageWarning,"InvalidColormapIndex","`%s'",image->filename);
  return(status);
}