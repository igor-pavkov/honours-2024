MagickExport Image *NewMagickImage(const ImageInfo *image_info,
  const size_t width,const size_t height,const PixelInfo *background,
  ExceptionInfo *exception)
{
  CacheView
    *image_view;

  Image
    *image;

  MagickBooleanType
    status;

  ssize_t
    y;

  assert(image_info != (const ImageInfo *) NULL);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(image_info->signature == MagickCoreSignature);
  assert(background != (const PixelInfo *) NULL);
  image=AcquireImage(image_info,exception);
  image->columns=width;
  image->rows=height;
  image->colorspace=background->colorspace;
  image->alpha_trait=background->alpha_trait;
  image->fuzz=background->fuzz;
  image->depth=background->depth;
  status=MagickTrue;
  image_view=AcquireAuthenticCacheView(image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(status) \
    magick_number_threads(image,image,image->rows,1)
#endif
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    register Quantum
      *magick_restrict q;

    register ssize_t
      x;

    if (status == MagickFalse)
      continue;
    q=QueueCacheViewAuthenticPixels(image_view,0,y,image->columns,1,exception);
    if (q == (Quantum *) NULL)
      {
        status=MagickFalse;
        continue;
      }
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      SetPixelViaPixelInfo(image,background,q);
      q+=GetPixelChannels(image);
    }
    if (SyncCacheViewAuthenticPixels(image_view,exception) == MagickFalse)
      status=MagickFalse;
  }
  image_view=DestroyCacheView(image_view);
  if (status == MagickFalse)
    image=DestroyImage(image);
  return(image);
}