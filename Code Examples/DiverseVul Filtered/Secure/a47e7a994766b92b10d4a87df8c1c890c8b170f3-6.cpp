static MagickBooleanType ChannelImage(Image *destination_image,
  const PixelChannel destination_channel,const ChannelFx channel_op,
  const Image *source_image,const PixelChannel source_channel,
  const Quantum pixel,ExceptionInfo *exception)
{
  CacheView
    *source_view,
    *destination_view;

  MagickBooleanType
    status;

  size_t
    height,
    width;

  ssize_t
    y;

  status=MagickTrue;
  source_view=AcquireVirtualCacheView(source_image,exception);
  destination_view=AcquireAuthenticCacheView(destination_image,exception);
  height=MagickMin(source_image->rows,destination_image->rows);
  width=MagickMin(source_image->columns,destination_image->columns);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(status) \
    magick_number_threads(source_image,source_image,height,1)
#endif
  for (y=0; y < (ssize_t) height; y++)
  {
    PixelTrait
      destination_traits,
      source_traits;

    register const Quantum
      *magick_restrict p;

    register Quantum
      *magick_restrict q;

    register ssize_t
      x;

    if (status == MagickFalse)
      continue;
    p=GetCacheViewVirtualPixels(source_view,0,y,source_image->columns,1,
      exception);
    q=GetCacheViewAuthenticPixels(destination_view,0,y,
      destination_image->columns,1,exception);
    if ((p == (const Quantum *) NULL) || (q == (Quantum *) NULL))
      {
        status=MagickFalse;
        continue;
      }
    destination_traits=GetPixelChannelTraits(destination_image,
      destination_channel);
    source_traits=GetPixelChannelTraits(source_image,source_channel);
    if ((destination_traits == UndefinedPixelTrait) ||
        (source_traits == UndefinedPixelTrait))
      continue;
    for (x=0; x < (ssize_t) width; x++)
    {
      if (channel_op == AssignChannelOp)
        SetPixelChannel(destination_image,destination_channel,pixel,q);
      else
        SetPixelChannel(destination_image,destination_channel,
          GetPixelChannel(source_image,source_channel,p),q);
      p+=GetPixelChannels(source_image);
      q+=GetPixelChannels(destination_image);
    }
    if (SyncCacheViewAuthenticPixels(destination_view,exception) == MagickFalse)
      status=MagickFalse;
  }
  destination_view=DestroyCacheView(destination_view);
  source_view=DestroyCacheView(source_view);
  return(status);
}