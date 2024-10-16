MagickExport MagickBooleanType SetImageRegionMask(Image *image,
  const PixelMask type,const RectangleInfo *region,ExceptionInfo *exception)
{
  CacheView
    *image_view;

  MagickBooleanType
    status;

  ssize_t
    y;

  /*
    Set image mask as defined by the region.
  */
  assert(image != (Image *) NULL);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(image->signature == MagickCoreSignature);
  if (region == (const RectangleInfo *) NULL)
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
          break;
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
    q=GetCacheViewAuthenticPixels(image_view,0,y,image->columns,1,exception);
    if (q == (Quantum *) NULL)
      {
        status=MagickFalse;
        continue;
      }
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      Quantum
        pixel;

      pixel=QuantumRange;
      if (((x >= region->x) && (x < (region->x+(ssize_t) region->width))) &&
          ((y >= region->y) && (y < (region->y+(ssize_t) region->height))))
        pixel=(Quantum) 0;
      switch (type)
      {
        case ReadPixelMask:
        {
          SetPixelReadMask(image,pixel,q);
          break;
        }
        case WritePixelMask:
        {
          SetPixelWriteMask(image,pixel,q);
          break;
        }
        default:
        {
          SetPixelCompositeMask(image,pixel,q);
          break;
        }
      }
      q+=GetPixelChannels(image);
    }
    if (SyncCacheViewAuthenticPixels(image_view,exception) == MagickFalse)
      status=MagickFalse;
  }
  image->mask_trait=UndefinedPixelTrait;
  image_view=DestroyCacheView(image_view);
  return(status);
}