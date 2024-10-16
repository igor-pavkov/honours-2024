MagickExport Image *SeparateImage(const Image *image,
  const ChannelType channel_type,ExceptionInfo *exception)
{
#define GetChannelBit(mask,bit)  (((size_t) (mask) >> (size_t) (bit)) & 0x01)
#define SeparateImageTag  "Separate/Image"

  CacheView
    *image_view,
    *separate_view;

  Image
    *separate_image;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  ssize_t
    y;

  /*
    Initialize separate image attributes.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  separate_image=CloneImage(image,0,0,MagickTrue,exception);
  if (separate_image == (Image *) NULL)
    return((Image *) NULL);
  if (SetImageStorageClass(separate_image,DirectClass,exception) == MagickFalse)
    {
      separate_image=DestroyImage(separate_image);
      return((Image *) NULL);
    }
  separate_image->alpha_trait=UndefinedPixelTrait;
  (void) SetImageColorspace(separate_image,GRAYColorspace,exception);
  separate_image->gamma=image->gamma;
  /*
    Separate image.
  */
  status=MagickTrue;
  progress=0;
  image_view=AcquireVirtualCacheView(image,exception);
  separate_view=AcquireAuthenticCacheView(separate_image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(progress,status) \
    magick_number_threads(image,image,image->rows,1)
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
    p=GetCacheViewVirtualPixels(image_view,0,y,image->columns,1,exception);
    q=QueueCacheViewAuthenticPixels(separate_view,0,y,separate_image->columns,1,
      exception);
    if ((p == (const Quantum *) NULL) || (q == (Quantum *) NULL))
      {
        status=MagickFalse;
        continue;
      }
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      register ssize_t
        i;

      SetPixelChannel(separate_image,GrayPixelChannel,(Quantum) 0,q);
      for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
      {
        PixelChannel channel = GetPixelChannelChannel(image,i);
        PixelTrait traits = GetPixelChannelTraits(image,channel);
        if ((traits == UndefinedPixelTrait) ||
            (GetChannelBit(channel_type,channel) == 0))
          continue;
        SetPixelChannel(separate_image,GrayPixelChannel,p[i],q);
      }
      p+=GetPixelChannels(image);
      q+=GetPixelChannels(separate_image);
    }
    if (SyncCacheViewAuthenticPixels(separate_view,exception) == MagickFalse)
      status=MagickFalse;
    if (image->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
        #pragma omp atomic
#endif
        progress++;
        proceed=SetImageProgress(image,SeparateImageTag,progress,image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  separate_view=DestroyCacheView(separate_view);
  image_view=DestroyCacheView(image_view);
  (void) SetImageChannelMask(separate_image,DefaultChannels);
  if (status == MagickFalse)
    separate_image=DestroyImage(separate_image);
  return(separate_image);
}