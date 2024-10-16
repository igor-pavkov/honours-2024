MagickExport Image *LiquidRescaleImage(const Image *image,const size_t columns,
  const size_t rows,const double delta_x,const double rigidity,
  ExceptionInfo *exception)
{
#define LiquidRescaleImageTag  "Rescale/Image"

  CacheView
    *image_view,
    *rescale_view;

  gfloat
    *packet,
    *pixels;

  Image
    *rescale_image;

  int
    x_offset,
    y_offset;

  LqrCarver
    *carver;

  LqrRetVal
    lqr_status;

  MagickBooleanType
    status;

  MemoryInfo
    *pixel_info;

  register gfloat
    *q;

  ssize_t
    y;

  /*
    Liquid rescale image.
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
  if ((columns <= 2) || (rows <= 2))
    return(ResizeImage(image,columns,rows,image->filter,exception));
  pixel_info=AcquireVirtualMemory(image->columns,image->rows*MaxPixelChannels*
    sizeof(*pixels));
  if (pixel_info == (MemoryInfo *) NULL)
    return((Image *) NULL);
  pixels=(gfloat *) GetVirtualMemoryBlob(pixel_info);
  status=MagickTrue;
  q=pixels;
  image_view=AcquireVirtualCacheView(image,exception);
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    register const Quantum
      *magick_restrict p;

    register ssize_t
      x;

    if (status == MagickFalse)
      continue;
    p=GetCacheViewVirtualPixels(image_view,0,y,image->columns,1,exception);
    if (p == (const Quantum *) NULL)
      {
        status=MagickFalse;
        continue;
      }
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      register ssize_t
        i;

      for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
        *q++=QuantumScale*p[i];
      p+=GetPixelChannels(image);
    }
  }
  image_view=DestroyCacheView(image_view);
  carver=lqr_carver_new_ext(pixels,(int) image->columns,(int) image->rows,
    (int) GetPixelChannels(image),LQR_COLDEPTH_32F);
  if (carver == (LqrCarver *) NULL)
    {
      pixel_info=RelinquishVirtualMemory(pixel_info);
      ThrowImageException(ResourceLimitError,"MemoryAllocationFailed");
    }
  lqr_carver_set_preserve_input_image(carver);
  lqr_status=lqr_carver_init(carver,(int) delta_x,rigidity);
  lqr_status=lqr_carver_resize(carver,(int) columns,(int) rows);
  (void) lqr_status;
  rescale_image=CloneImage(image,lqr_carver_get_width(carver),
    lqr_carver_get_height(carver),MagickTrue,exception);
  if (rescale_image == (Image *) NULL)
    {
      pixel_info=RelinquishVirtualMemory(pixel_info);
      return((Image *) NULL);
    }
  if (SetImageStorageClass(rescale_image,DirectClass,exception) == MagickFalse)
    {
      pixel_info=RelinquishVirtualMemory(pixel_info);
      rescale_image=DestroyImage(rescale_image);
      return((Image *) NULL);
    }
  rescale_view=AcquireAuthenticCacheView(rescale_image,exception);
  (void) lqr_carver_scan_reset(carver);
  while (lqr_carver_scan_ext(carver,&x_offset,&y_offset,(void **) &packet) != 0)
  {
    register Quantum
      *magick_restrict p;

    register ssize_t
      i;

    p=QueueCacheViewAuthenticPixels(rescale_view,x_offset,y_offset,1,1,
      exception);
    if (p == (Quantum *) NULL)
      break;
    for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
    {
      PixelChannel
        channel;

      PixelTrait
        rescale_traits,
        traits;

      channel=GetPixelChannelChannel(image,i);
      traits=GetPixelChannelTraits(image,channel);
      rescale_traits=GetPixelChannelTraits(rescale_image,channel);
      if ((traits == UndefinedPixelTrait) ||
          (rescale_traits == UndefinedPixelTrait))
        continue;
      SetPixelChannel(rescale_image,channel,ClampToQuantum(QuantumRange*
        packet[i]),p);
    }
    if (SyncCacheViewAuthenticPixels(rescale_view,exception) == MagickFalse)
      break;
  }
  rescale_view=DestroyCacheView(rescale_view);
  pixel_info=RelinquishVirtualMemory(pixel_info);
  lqr_carver_destroy(carver);
  return(rescale_image);
}