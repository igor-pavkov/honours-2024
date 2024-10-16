MagickExport Image *MagnifyImage(const Image *image,ExceptionInfo *exception)
{
#define MagnifyImageTag  "Magnify/Image"

  CacheView
    *image_view,
    *magnify_view;

  Image
    *magnify_image;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  ssize_t
    y;

  /*
    Initialize magnified image attributes.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  magnify_image=CloneImage(image,2*image->columns,2*image->rows,MagickTrue,
    exception);
  if (magnify_image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Magnify image.
  */
  status=MagickTrue;
  progress=0;
  image_view=AcquireVirtualCacheView(image,exception);
  magnify_view=AcquireAuthenticCacheView(magnify_image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static,4) shared(progress,status) \
    magick_threads(image,magnify_image,image->rows,1)
#endif
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    register Quantum
      *magick_restrict q;

    register ssize_t
      x;

    if (status == MagickFalse)
      continue;
    q=QueueCacheViewAuthenticPixels(magnify_view,0,2*y,magnify_image->columns,2,
      exception);
    if (q == (Quantum *) NULL)
      {
        status=MagickFalse;
        continue;
      }
    /*
      Magnify this row of pixels.
    */
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      MagickRealType
        intensity[9];

      register const Quantum
        *magick_restrict p;

      register Quantum
        *magick_restrict r;

      register ssize_t
        i;

      size_t
        channels;

      p=GetCacheViewVirtualPixels(image_view,x-1,y-1,3,3,exception);
      if (p == (const Quantum *) NULL)
        {
          status=MagickFalse;
          continue;
        }
      channels=GetPixelChannels(image);
      for (i=0; i < 9; i++)
        intensity[i]=GetPixelIntensity(image,p+i*channels);
      r=q;
      if ((fabs(intensity[1]-intensity[7]) < MagickEpsilon) ||
          (fabs(intensity[3]-intensity[5]) < MagickEpsilon))
        {
          /*
            Clone center pixel.
          */
          for (i=0; i < (ssize_t) channels; i++)
            r[i]=p[4*channels+i];
          r+=GetPixelChannels(magnify_image);
          for (i=0; i < (ssize_t) channels; i++)
            r[i]=p[4*channels+i];
          r+=GetPixelChannels(magnify_image)*(magnify_image->columns-1);
          for (i=0; i < (ssize_t) channels; i++)
            r[i]=p[4*channels+i];
          r+=GetPixelChannels(magnify_image);
          for (i=0; i < (ssize_t) channels; i++)
            r[i]=p[4*channels+i];
        }
      else
        {
          /*
            Selectively clone pixel.
          */
          if (fabs(intensity[1]-intensity[3]) < MagickEpsilon)
            for (i=0; i < (ssize_t) channels; i++)
              r[i]=p[3*channels+i];
          else
            for (i=0; i < (ssize_t) channels; i++)
              r[i]=p[4*channels+i];
          r+=GetPixelChannels(magnify_image);
          if (fabs(intensity[1]-intensity[5]) < MagickEpsilon)
            for (i=0; i < (ssize_t) channels; i++)
              r[i]=p[5*channels+i];
          else
            for (i=0; i < (ssize_t) channels; i++)
              r[i]=p[4*channels+i];
          r+=GetPixelChannels(magnify_image)*(magnify_image->columns-1);
          if (fabs(intensity[3]-intensity[7]) < MagickEpsilon)
            for (i=0; i < (ssize_t) channels; i++)
              r[i]=p[3*channels+i];
          else
            for (i=0; i < (ssize_t) channels; i++)
              r[i]=p[4*channels+i];
          r+=GetPixelChannels(magnify_image);
          if (fabs(intensity[5]-intensity[7]) < MagickEpsilon)
            for (i=0; i < (ssize_t) channels; i++)
              r[i]=p[5*channels+i];
          else
            for (i=0; i < (ssize_t) channels; i++)
              r[i]=p[4*channels+i];
        }
      q+=2*GetPixelChannels(magnify_image);
    }
    if (SyncCacheViewAuthenticPixels(magnify_view,exception) == MagickFalse)
      status=MagickFalse;
    if (image->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
        #pragma omp critical (MagickCore_MagnifyImage)
#endif
        proceed=SetImageProgress(image,MagnifyImageTag,progress++,image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  magnify_view=DestroyCacheView(magnify_view);
  image_view=DestroyCacheView(image_view);
  if (status == MagickFalse)
    magnify_image=DestroyImage(magnify_image);
  return(magnify_image);
}