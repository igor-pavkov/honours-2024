MagickExport MagickBooleanType IsHighDynamicRangeImage(const Image *image,
  ExceptionInfo *exception)
{
#if !defined(MAGICKCORE_HDRI_SUPPORT)
  (void) image;
  (void) exception;
  return(MagickFalse);
#else
  CacheView
    *image_view;

  MagickBooleanType
    status;

  ssize_t
    y;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  status=MagickTrue;
  image_view=AcquireVirtualCacheView(image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(status) \
    magick_number_threads(image,image,image->rows,1)
#endif
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    register const Quantum
      *p;

    register ssize_t
      x;

    if (status == MagickFalse)
      continue;
    p=GetCacheViewVirtualPixels(image_view,0,y,image->columns,1,exception);
    if (p == (const Quantum *) NULL)
      {
        status=MagickFalse;
        continue;
      }
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      register ssize_t
        i;

      for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
      {
        double
          pixel;

        PixelTrait
          traits;

        traits=GetPixelChannelTraits(image,(PixelChannel) i);
        if (traits == UndefinedPixelTrait)
          continue;
        pixel=(double) p[i];
        if ((pixel < 0.0) || (pixel > QuantumRange) ||
            (pixel != (double) ((QuantumAny) pixel)))
          break;
      }
      p+=GetPixelChannels(image);
      if (i < (ssize_t) GetPixelChannels(image))
        status=MagickFalse;
    }
    if (x < (ssize_t) image->columns)
      status=MagickFalse;
  }
  image_view=DestroyCacheView(image_view);
  return(status != MagickFalse ? MagickFalse : MagickTrue);
#endif
}