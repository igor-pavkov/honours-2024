MagickExport Image *CombineImages(const Image *image,
  const ColorspaceType colorspace,ExceptionInfo *exception)
{
#define CombineImageTag  "Combine/Image"

  CacheView
    *combine_view;

  Image
    *combine_image;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  ssize_t
    y;

  /*
    Ensure the image are the same size.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  combine_image=CloneImage(image,0,0,MagickTrue,exception);
  if (combine_image == (Image *) NULL)
    return((Image *) NULL);
  if (SetImageStorageClass(combine_image,DirectClass,exception) == MagickFalse)
    {
      combine_image=DestroyImage(combine_image);
      return((Image *) NULL);
    }
  if (colorspace != UndefinedColorspace)
    (void) SetImageColorspace(combine_image,colorspace,exception);
  else
    if (fabs(image->gamma-1.0) <= MagickEpsilon)
      (void) SetImageColorspace(combine_image,RGBColorspace,exception);
    else
      (void) SetImageColorspace(combine_image,sRGBColorspace,exception);
  switch (combine_image->colorspace)
  {
    case UndefinedColorspace:
    case sRGBColorspace:
    {
      if (GetImageListLength(image) > 3)
        combine_image->alpha_trait=BlendPixelTrait;
      break;
    }
    case LinearGRAYColorspace:
    case GRAYColorspace:
    {
      if (GetImageListLength(image) > 1)
        combine_image->alpha_trait=BlendPixelTrait;
      break;
    }
    case CMYKColorspace:
    {
      if (GetImageListLength(image) > 4)
        combine_image->alpha_trait=BlendPixelTrait;
      break;
    }
    default:
      break;
  }
  /*
    Combine images.
  */
  status=MagickTrue;
  progress=0;
  combine_view=AcquireAuthenticCacheView(combine_image,exception);
  for (y=0; y < (ssize_t) combine_image->rows; y++)
  {
    CacheView
      *image_view;

    const Image
      *next;

    Quantum
      *pixels;

    register const Quantum
      *magick_restrict p;

    register Quantum
      *magick_restrict q;

    register ssize_t
      i;

    if (status == MagickFalse)
      continue;
    pixels=GetCacheViewAuthenticPixels(combine_view,0,y,combine_image->columns,
      1,exception);
    if (pixels == (Quantum *) NULL)
      {
        status=MagickFalse;
        continue;
      }
    next=image;
    for (i=0; i < (ssize_t) GetPixelChannels(combine_image); i++)
    {
      register ssize_t
        x;

      PixelChannel channel = GetPixelChannelChannel(combine_image,i);
      PixelTrait traits = GetPixelChannelTraits(combine_image,channel);
      if (traits == UndefinedPixelTrait)
        continue;
      if (next == (Image *) NULL)
        continue;
      image_view=AcquireVirtualCacheView(next,exception);
      p=GetCacheViewVirtualPixels(image_view,0,y,next->columns,1,exception);
      if (p == (const Quantum *) NULL)
        continue;
      q=pixels;
      for (x=0; x < (ssize_t) combine_image->columns; x++)
      {
        if (x < (ssize_t) next->columns)
          {
            q[i]=GetPixelGray(next,p);
            p+=GetPixelChannels(next);
          }
        q+=GetPixelChannels(combine_image);
      }
      image_view=DestroyCacheView(image_view);
      next=GetNextImageInList(next);
    }
    if (SyncCacheViewAuthenticPixels(combine_view,exception) == MagickFalse)
      status=MagickFalse;
    if (image->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
        #pragma omp atomic
#endif
        progress++;
        proceed=SetImageProgress(image,CombineImageTag,progress,
          combine_image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  combine_view=DestroyCacheView(combine_view);
  if (status == MagickFalse)
    combine_image=DestroyImage(combine_image);
  return(combine_image);
}