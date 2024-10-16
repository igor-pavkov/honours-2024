MagickExport MagickBooleanType SetImageMask(Image *image,const PixelMask type,
  const Image *mask,ExceptionInfo *exception)
{
  CacheView
    *mask_view,
    *image_view;

  MagickBooleanType
    status;

  ssize_t
    y;

  /*
    Set image mask.
  */
  assert(image != (Image *) NULL);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(image->signature == MagickCoreSignature);
  if (mask == (const Image *) NULL)
    {
      switch (type)
      {
        case ReadPixelMask:
        {
          image->channels=(ChannelType) (image->channels & ~ReadMaskChannel);
          break;
        }
        case WritePixelMask:
        {
          image->channels=(ChannelType) (image->channels & ~WriteMaskChannel);
        }
        default:
        {
          image->channels=(ChannelType) (image->channels & ~CompositeMaskChannel);
          break;
        }
      }
      return(SyncImagePixelCache(image,exception));
    }
  switch (type)
  {
    case ReadPixelMask:
    {
      image->channels=(ChannelType) (image->channels | ReadMaskChannel);
      break;
    }
    case WritePixelMask:
    {
      image->channels=(ChannelType) (image->channels | WriteMaskChannel);
      break;
    }
    default:
    {
      image->channels=(ChannelType) (image->channels | CompositeMaskChannel);
      break;
    }
  }
  if (SyncImagePixelCache(image,exception) == MagickFalse)
    return(MagickFalse);
  status=MagickTrue;
  image->mask_trait=UpdatePixelTrait;
  mask_view=AcquireVirtualCacheView(mask,exception);
  image_view=AcquireAuthenticCacheView(image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(status) \
    magick_number_threads(mask,image,image->rows,1)
#endif
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    register const Quantum
      *magick_restrict p;

    register Quantum
      *magick_restrict q;

    register ssize_t
      x;

    if (status == MagickFalse)
      continue;
    p=GetCacheViewVirtualPixels(mask_view,0,y,mask->columns,1,exception);
    q=GetCacheViewAuthenticPixels(image_view,0,y,image->columns,1,exception);
    if ((p == (const Quantum *) NULL) || (q == (Quantum *) NULL))
      {
        status=MagickFalse;
        continue;
      }
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      MagickRealType
        intensity;

      intensity=0.0;
      if ((x < (ssize_t) mask->columns) && (y < (ssize_t) mask->rows))
        intensity=GetPixelIntensity(mask,p);
      switch (type)
      {
        case ReadPixelMask:
        {
          SetPixelReadMask(image,ClampToQuantum(intensity),q);
          break;
        }
        case WritePixelMask:
        {
          SetPixelWriteMask(image,ClampToQuantum(intensity),q);
          break;
        }
        default:
        {
          SetPixelCompositeMask(image,ClampToQuantum(intensity),q);
          break;
        }
      }
      p+=GetPixelChannels(mask);
      q+=GetPixelChannels(image);
    }
    if (SyncCacheViewAuthenticPixels(image_view,exception) == MagickFalse)
      status=MagickFalse;
  }
  image->mask_trait=UndefinedPixelTrait;
  mask_view=DestroyCacheView(mask_view);
  image_view=DestroyCacheView(image_view);
  return(status);
}