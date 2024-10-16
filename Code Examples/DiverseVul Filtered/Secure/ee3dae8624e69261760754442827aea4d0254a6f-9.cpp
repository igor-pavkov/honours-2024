MagickExport MagickBooleanType CopyImagePixels(Image *image,
  const Image *source_image,const RectangleInfo *geometry,
  const OffsetInfo *offset,ExceptionInfo *exception)
{
#define CopyImageTag  "Copy/Image"

  CacheView
    *image_view,
    *source_view;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  ssize_t
    y;

  assert(image != (Image *) NULL);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(source_image != (Image *) NULL);
  assert(geometry != (RectangleInfo *) NULL);
  assert(offset != (OffsetInfo *) NULL);
  if ((offset->x < 0) || (offset->y < 0) ||
      ((ssize_t) (offset->x+geometry->width) > (ssize_t) image->columns) ||
      ((ssize_t) (offset->y+geometry->height) > (ssize_t) image->rows))
    ThrowBinaryException(OptionError,"GeometryDoesNotContainImage",
      image->filename);
  if (SetImageStorageClass(image,DirectClass,exception) == MagickFalse)
    return(MagickFalse);
  /*
    Copy image pixels.
  */
  status=MagickTrue;
  progress=0;
  source_view=AcquireVirtualCacheView(source_image,exception);
  image_view=AcquireAuthenticCacheView(image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(progress,status) \
    magick_number_threads(image,source_image,geometry->height,1)
#endif
  for (y=0; y < (ssize_t) geometry->height; y++)
  {
    MagickBooleanType
      sync;

    register const Quantum
      *magick_restrict p;

    register ssize_t
      x;

    register Quantum
      *magick_restrict q;

    if (status == MagickFalse)
      continue;
    p=GetCacheViewVirtualPixels(source_view,geometry->x,y+geometry->y,
      geometry->width,1,exception);
    q=QueueCacheViewAuthenticPixels(image_view,offset->x,y+offset->y,
      geometry->width,1,exception);
    if ((p == (const Quantum *) NULL) || (q == (Quantum *) NULL))
      {
        status=MagickFalse;
        continue;
      }
    for (x=0; x < (ssize_t) geometry->width; x++)
    {
      register ssize_t
        i;

      for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
      {
        PixelChannel channel = GetPixelChannelChannel(image,i);
        PixelTrait traits = GetPixelChannelTraits(image,channel);
        PixelTrait source_traits=GetPixelChannelTraits(source_image,channel);
        if ((traits == UndefinedPixelTrait) ||
            ((traits & UpdatePixelTrait) == 0) ||
            (source_traits == UndefinedPixelTrait))
          continue;
        SetPixelChannel(image,channel,p[i],q);
      }
      p+=GetPixelChannels(source_image);
      q+=GetPixelChannels(image);
    }
    sync=SyncCacheViewAuthenticPixels(image_view,exception);
    if (sync == MagickFalse)
      status=MagickFalse;
    if (image->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
        #pragma omp atomic
#endif
        progress++;
        proceed=SetImageProgress(image,CopyImageTag,progress,image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  source_view=DestroyCacheView(source_view);
  image_view=DestroyCacheView(image_view);
  return(status);
}