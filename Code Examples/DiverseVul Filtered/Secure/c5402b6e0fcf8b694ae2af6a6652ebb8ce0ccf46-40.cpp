MagickExport Image *InterpolativeResizeImage(const Image *image,
  const size_t columns,const size_t rows,const PixelInterpolateMethod method,
  ExceptionInfo *exception)
{
#define InterpolativeResizeImageTag  "Resize/Image"

  CacheView
    *image_view,
    *resize_view;

  Image
    *resize_image;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  PointInfo
    scale;

  ssize_t
    y;

  /*
    Interpolatively resize image.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  if ((columns == 0) || (rows == 0))
    ThrowImageException(ImageError,"NegativeOrZeroImageSize");
  if ((columns == image->columns) && (rows == image->rows))
    return(CloneImage(image,0,0,MagickTrue,exception));
  resize_image=CloneImage(image,columns,rows,MagickTrue,exception);
  if (resize_image == (Image *) NULL)
    return((Image *) NULL);
  if (SetImageStorageClass(resize_image,DirectClass,exception) == MagickFalse)
    {
      resize_image=DestroyImage(resize_image);
      return((Image *) NULL);
    }
  status=MagickTrue;
  progress=0;
  image_view=AcquireVirtualCacheView(image,exception);
  resize_view=AcquireAuthenticCacheView(resize_image,exception);
  scale.x=(double) image->columns/resize_image->columns;
  scale.y=(double) image->rows/resize_image->rows;
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static,4) shared(progress,status) \
    magick_threads(image,resize_image,resize_image->rows,1)
#endif
  for (y=0; y < (ssize_t) resize_image->rows; y++)
  {
    PointInfo
      offset;

    register Quantum
      *magick_restrict q;

    register ssize_t
      x;

    if (status == MagickFalse)
      continue;
    q=QueueCacheViewAuthenticPixels(resize_view,0,y,resize_image->columns,1,
      exception);
    if (q == (Quantum *) NULL)
      continue;
    offset.y=((double) y+0.5)*scale.y-0.5;
    for (x=0; x < (ssize_t) resize_image->columns; x++)
    {
      register ssize_t
        i;

      if (GetPixelWriteMask(resize_image,q) <= (QuantumRange/2))
        {
          q+=GetPixelChannels(resize_image);
          continue;
        }
      for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
      {
        PixelChannel
          channel;

        PixelTrait
          resize_traits,
          traits;

        channel=GetPixelChannelChannel(image,i);
        traits=GetPixelChannelTraits(image,channel);
        resize_traits=GetPixelChannelTraits(resize_image,channel);
        if ((traits == UndefinedPixelTrait) ||
            (resize_traits == UndefinedPixelTrait))
          continue;
        offset.x=((double) x+0.5)*scale.x-0.5;
        status=InterpolatePixelChannels(image,image_view,resize_image,method,
          offset.x,offset.y,q,exception);
      }
      q+=GetPixelChannels(resize_image);
    }
    if (SyncCacheViewAuthenticPixels(resize_view,exception) == MagickFalse)
      status=MagickFalse;
    if (image->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
        #pragma omp critical (MagickCore_InterpolativeResizeImage)
#endif
        proceed=SetImageProgress(image,InterpolativeResizeImageTag,progress++,
          image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  resize_view=DestroyCacheView(resize_view);
  image_view=DestroyCacheView(image_view);
  if (status == MagickFalse)
    resize_image=DestroyImage(resize_image);
  return(resize_image);
}