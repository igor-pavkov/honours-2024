MagickExport Image *WaveletDenoiseImage(const Image *image,
  const double threshold,const double softness,ExceptionInfo *exception)
{
  CacheView
    *image_view,
    *noise_view;

  float
    *kernel,
    *pixels;

  Image
    *noise_image;

  MagickBooleanType
    status;

  MagickSizeType
    number_pixels;

  MemoryInfo
    *pixels_info;

  ssize_t
    channel;

  static const float
    noise_levels[] = { 0.8002f, 0.2735f, 0.1202f, 0.0585f, 0.0291f, 0.0152f,
      0.0080f, 0.0044f };

  /*
    Initialize noise image attributes.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
#if defined(MAGICKCORE_OPENCL_SUPPORT)
  noise_image=AccelerateWaveletDenoiseImage(image,threshold,exception);
  if (noise_image != (Image *) NULL)
    return(noise_image);
#endif
  noise_image=CloneImage(image,0,0,MagickTrue,exception);
  if (noise_image == (Image *) NULL)
    return((Image *) NULL);
  if (SetImageStorageClass(noise_image,DirectClass,exception) == MagickFalse)
    {
      noise_image=DestroyImage(noise_image);
      return((Image *) NULL);
    }
  if (AcquireMagickResource(WidthResource,4*image->columns) == MagickFalse)
    ThrowImageException(ResourceLimitError,"MemoryAllocationFailed");
  pixels_info=AcquireVirtualMemory(3*image->columns,image->rows*
    sizeof(*pixels));
  kernel=(float *) AcquireQuantumMemory(MagickMax(image->rows,image->columns)+1,
    GetOpenMPMaximumThreads()*sizeof(*kernel));
  if ((pixels_info == (MemoryInfo *) NULL) || (kernel == (float *) NULL))
    {
      if (kernel != (float *) NULL)
        kernel=(float *) RelinquishMagickMemory(kernel);
      if (pixels_info != (MemoryInfo *) NULL)
        pixels_info=RelinquishVirtualMemory(pixels_info);
      ThrowImageException(ResourceLimitError,"MemoryAllocationFailed");
    }
  pixels=(float *) GetVirtualMemoryBlob(pixels_info);
  status=MagickTrue;
  number_pixels=(MagickSizeType) image->columns*image->rows;
  image_view=AcquireAuthenticCacheView(image,exception);
  noise_view=AcquireAuthenticCacheView(noise_image,exception);
  for (channel=0; channel < (ssize_t) GetPixelChannels(image); channel++)
  {
    ssize_t
      i;

    size_t
      high_pass,
      low_pass;

    ssize_t
      level,
      y;

    PixelChannel
      pixel_channel;

    PixelTrait
      traits;

    if (status == MagickFalse)
      continue;
    traits=GetPixelChannelTraits(image,(PixelChannel) channel);
    if (traits == UndefinedPixelTrait)
      continue;
    pixel_channel=GetPixelChannelChannel(image,channel);
    if ((pixel_channel != RedPixelChannel) &&
        (pixel_channel != GreenPixelChannel) &&
        (pixel_channel != BluePixelChannel))
      continue;
    /*
      Copy channel from image to wavelet pixel array.
    */
    i=0;
    for (y=0; y < (ssize_t) image->rows; y++)
    {
      const Quantum
        *magick_restrict p;

      ssize_t
        x;

      p=GetCacheViewAuthenticPixels(image_view,0,y,image->columns,1,exception);
      if (p == (const Quantum *) NULL)
        {
          status=MagickFalse;
          break;
        }
      for (x=0; x < (ssize_t) image->columns; x++)
      {
        pixels[i++]=(float) p[channel];
        p+=GetPixelChannels(image);
      }
    }
    /*
      Low pass filter outputs are called approximation kernel & high pass
      filters are referred to as detail kernel. The detail kernel
      have high values in the noisy parts of the signal.
    */
    high_pass=0;
    for (level=0; level < 5; level++)
    {
      double
        magnitude;

      ssize_t
        x,
        y;

      low_pass=(size_t) (number_pixels*((level & 0x01)+1));
#if defined(MAGICKCORE_OPENMP_SUPPORT)
      #pragma omp parallel for schedule(static,1) \
        magick_number_threads(image,image,image->rows,1)
#endif
      for (y=0; y < (ssize_t) image->rows; y++)
      {
        const int
          id = GetOpenMPThreadId();

        float
          *magick_restrict p,
          *magick_restrict q;

        ssize_t
          x;

        p=kernel+id*image->columns;
        q=pixels+y*image->columns;
        HatTransform(q+high_pass,1,image->columns,(size_t) (1UL << level),p);
        q+=low_pass;
        for (x=0; x < (ssize_t) image->columns; x++)
          *q++=(*p++);
      }
#if defined(MAGICKCORE_OPENMP_SUPPORT)
      #pragma omp parallel for schedule(static,1) \
        magick_number_threads(image,image,image->columns,1)
#endif
      for (x=0; x < (ssize_t) image->columns; x++)
      {
        const int
          id = GetOpenMPThreadId();

        float
          *magick_restrict p,
          *magick_restrict q;

        ssize_t
          y;

        p=kernel+id*image->rows;
        q=pixels+x+low_pass;
        HatTransform(q,image->columns,image->rows,(size_t) (1UL << level),p);
        for (y=0; y < (ssize_t) image->rows; y++)
        {
          *q=(*p++);
          q+=image->columns;
        }
      }
      /*
        To threshold, each coefficient is compared to a threshold value and
        attenuated / shrunk by some factor.
      */
      magnitude=threshold*noise_levels[level];
      for (i=0; i < (ssize_t) number_pixels; ++i)
      {
        pixels[high_pass+i]-=pixels[low_pass+i];
        if (pixels[high_pass+i] < -magnitude)
          pixels[high_pass+i]+=magnitude-softness*magnitude;
        else
          if (pixels[high_pass+i] > magnitude)
            pixels[high_pass+i]-=magnitude-softness*magnitude;
          else
            pixels[high_pass+i]*=softness;
        if (high_pass != 0)
          pixels[i]+=pixels[high_pass+i];
      }
      high_pass=low_pass;
    }
    /*
      Reconstruct image from the thresholded wavelet kernel.
    */
    i=0;
    for (y=0; y < (ssize_t) image->rows; y++)
    {
      MagickBooleanType
        sync;

      Quantum
        *magick_restrict q;

      ssize_t
        x;

      ssize_t
        offset;

      q=GetCacheViewAuthenticPixels(noise_view,0,y,noise_image->columns,1,
        exception);
      if (q == (Quantum *) NULL)
        {
          status=MagickFalse;
          break;
        }
      offset=GetPixelChannelOffset(noise_image,pixel_channel);
      for (x=0; x < (ssize_t) image->columns; x++)
      {
        MagickRealType
          pixel;

        pixel=(MagickRealType) pixels[i]+pixels[low_pass+i];
        q[offset]=ClampToQuantum(pixel);
        i++;
        q+=GetPixelChannels(noise_image);
      }
      sync=SyncCacheViewAuthenticPixels(noise_view,exception);
      if (sync == MagickFalse)
        status=MagickFalse;
    }
    if (image->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

        proceed=SetImageProgress(image,AddNoiseImageTag,(MagickOffsetType)
          channel,GetPixelChannels(image));
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  noise_view=DestroyCacheView(noise_view);
  image_view=DestroyCacheView(image_view);
  kernel=(float *) RelinquishMagickMemory(kernel);
  pixels_info=RelinquishVirtualMemory(pixels_info);
  if (status == MagickFalse)
    noise_image=DestroyImage(noise_image);
  return(noise_image);
}