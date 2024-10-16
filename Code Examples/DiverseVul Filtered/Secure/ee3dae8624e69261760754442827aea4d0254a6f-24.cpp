MagickExport Image *GetImageMask(const Image *image,const PixelMask type,
  ExceptionInfo *exception)
{
  CacheView
    *mask_view,
    *image_view;

  Image
    *mask_image;

  MagickBooleanType
    status;

  ssize_t
    y;

  /*
    Get image mask.
  */
  assert(image != (Image *) NULL);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(image->signature == MagickCoreSignature);
  switch (type)
  {
    case ReadPixelMask:
    {
      if ((image->channels & ReadMaskChannel) == 0)
        return((Image *) NULL);
      break;
    }
    case WritePixelMask:
    {
      if ((image->channels & WriteMaskChannel) == 0)
        return((Image *) NULL);
      break;
    }
    default:
    {
      if ((image->channels & CompositeMaskChannel) == 0)
        return((Image *) NULL);
      break;
    }
  }
  mask_image=AcquireImage((ImageInfo *) NULL,exception);
  status=SetImageExtent(mask_image,image->columns,image->rows,exception);
  if (status == MagickFalse)
    return(DestroyImage(mask_image));
  status=MagickTrue;
  mask_image->alpha_trait=UndefinedPixelTrait;
  (void) SetImageColorspace(mask_image,GRAYColorspace,exception);
  image_view=AcquireVirtualCacheView(image,exception);
  mask_view=AcquireAuthenticCacheView(mask_image,exception);
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
    q=GetCacheViewAuthenticPixels(mask_view,0,y,mask_image->columns,1,
      exception);
    if ((p == (const Quantum *) NULL) || (q == (Quantum *) NULL))
      {
        status=MagickFalse;
        continue;
      }
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      switch (type)
      {
        case ReadPixelMask:
        {
          SetPixelGray(mask_image,GetPixelReadMask(image,p),q);
          break;
        }
        case WritePixelMask:
        {
          SetPixelGray(mask_image,GetPixelWriteMask(image,p),q);
          break;
        }
        default:
        {
          SetPixelGray(mask_image,GetPixelCompositeMask(image,p),q);
          break;
        }
      }
      p+=GetPixelChannels(image);
      q+=GetPixelChannels(mask_image);
    }
    if (SyncCacheViewAuthenticPixels(mask_view,exception) == MagickFalse)
      status=MagickFalse;
  }
  mask_view=DestroyCacheView(mask_view);
  image_view=DestroyCacheView(image_view);
  if (status == MagickFalse)
    mask_image=DestroyImage(mask_image);
  return(mask_image);
}