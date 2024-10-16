MagickExport Image *AppendImages(const Image *images,
  const MagickBooleanType stack,ExceptionInfo *exception)
{
#define AppendImageTag  "Append/Image"

  CacheView
    *append_view;

  Image
    *append_image;

  MagickBooleanType
    homogeneous_colorspace,
    status;

  MagickOffsetType
    n;

  PixelTrait
    alpha_trait;

  RectangleInfo
    geometry;

  register const Image
    *next;

  size_t
    depth,
    height,
    number_images,
    width;

  ssize_t
    x_offset,
    y,
    y_offset;

  /*
    Compute maximum area of appended area.
  */
  assert(images != (Image *) NULL);
  assert(images->signature == MagickCoreSignature);
  if (images->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",images->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  alpha_trait=images->alpha_trait;
  number_images=1;
  width=images->columns;
  height=images->rows;
  depth=images->depth;
  homogeneous_colorspace=MagickTrue;
  next=GetNextImageInList(images);
  for ( ; next != (Image *) NULL; next=GetNextImageInList(next))
  {
    if (next->depth > depth)
      depth=next->depth;
    if (next->colorspace != images->colorspace)
      homogeneous_colorspace=MagickFalse;
    if (next->alpha_trait != UndefinedPixelTrait)
      alpha_trait=BlendPixelTrait;
    number_images++;
    if (stack != MagickFalse)
      {
        if (next->columns > width)
          width=next->columns;
        height+=next->rows;
        continue;
      }
    width+=next->columns;
    if (next->rows > height)
      height=next->rows;
  }
  /*
    Append images.
  */
  append_image=CloneImage(images,width,height,MagickTrue,exception);
  if (append_image == (Image *) NULL)
    return((Image *) NULL);
  if (SetImageStorageClass(append_image,DirectClass,exception) == MagickFalse)
    {
      append_image=DestroyImage(append_image);
      return((Image *) NULL);
    }
  if (homogeneous_colorspace == MagickFalse)
    (void) SetImageColorspace(append_image,sRGBColorspace,exception);
  append_image->depth=depth;
  append_image->alpha_trait=alpha_trait;
  append_image->page=images->page;
  (void) SetImageBackgroundColor(append_image,exception);
  status=MagickTrue;
  x_offset=0;
  y_offset=0;
  next=images;
  append_view=AcquireAuthenticCacheView(append_image,exception);
  for (n=0; n < (MagickOffsetType) number_images; n++)
  {
    CacheView
      *image_view;

    MagickBooleanType
      proceed;

    SetGeometry(append_image,&geometry);
    GravityAdjustGeometry(next->columns,next->rows,next->gravity,&geometry);
    if (stack != MagickFalse)
      x_offset-=geometry.x;
    else
      y_offset-=geometry.y;
    image_view=AcquireVirtualCacheView(next,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
    #pragma omp parallel for schedule(static) shared(status) \
      magick_number_threads(next,next,next->rows,1)
#endif
    for (y=0; y < (ssize_t) next->rows; y++)
    {
      MagickBooleanType
        sync;

      PixelInfo
        pixel;

      register const Quantum
        *magick_restrict p;

      register Quantum
        *magick_restrict q;

      register ssize_t
        x;

      if (status == MagickFalse)
        continue;
      p=GetCacheViewVirtualPixels(image_view,0,y,next->columns,1,exception);
      q=QueueCacheViewAuthenticPixels(append_view,x_offset,y+y_offset,
        next->columns,1,exception);
      if ((p == (const Quantum *) NULL) || (q == (Quantum *) NULL))
        {
          status=MagickFalse;
          continue;
        }
      GetPixelInfo(next,&pixel);
      for (x=0; x < (ssize_t) next->columns; x++)
      {
        GetPixelInfoPixel(next,p,&pixel);
        SetPixelViaPixelInfo(append_image,&pixel,q);
        p+=GetPixelChannels(next);
        q+=GetPixelChannels(append_image);
      }
      sync=SyncCacheViewAuthenticPixels(append_view,exception);
      if (sync == MagickFalse)
        status=MagickFalse;
    }
    image_view=DestroyCacheView(image_view);
    if (stack == MagickFalse)
      {
        x_offset+=(ssize_t) next->columns;
        y_offset=0;
      }
    else
      {
        x_offset=0;
        y_offset+=(ssize_t) next->rows;
      }
    proceed=SetImageProgress(append_image,AppendImageTag,n,number_images);
    if (proceed == MagickFalse)
      break;
    next=GetNextImageInList(next);
  }
  append_view=DestroyCacheView(append_view);
  if (status == MagickFalse)
    append_image=DestroyImage(append_image);
  return(append_image);
}