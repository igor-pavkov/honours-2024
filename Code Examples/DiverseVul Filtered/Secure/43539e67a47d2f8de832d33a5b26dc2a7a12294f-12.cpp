MagickExport Image *MagnifyImage(const Image *image,ExceptionInfo *exception)
{
#define MagnifyImageTag  "Magnify/Image"

  CacheView
    *image_view,
    *magnify_view;

  const char
    *option;

  Image
    *source_image,
    *magnify_image;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  OffsetInfo
    offset;

  RectangleInfo
    rectangle;

  ssize_t
    y;

  unsigned char
    magnification,
    width;

  void
    (*scaling_method)(const Image *,const Quantum *,Quantum *,size_t);

  /*
    Initialize magnified image attributes.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  option=GetImageOption(image->image_info,"magnify:method");
  if (option == (char *) NULL)
    option="scale2x";
  scaling_method=Scale2X;
  magnification=1;
  width=1;
  switch (*option)
  {
    case 'e':
    {
      if (LocaleCompare(option,"eagle2x") == 0)
        {
          scaling_method=Eagle2X;
          magnification=2;
          width=3;
          break;
        }
      if (LocaleCompare(option,"eagle3x") == 0)
        {
          scaling_method=Eagle3X;
          magnification=3;
          width=3;
          break;
        }
      if (LocaleCompare(option,"eagle3xb") == 0)
        {
          scaling_method=Eagle3XB;
          magnification=3;
          width=3;
          break;
        }
      if (LocaleCompare(option,"epbx2x") == 0)
        {
          scaling_method=Epbx2X;
          magnification=2;
          width=3;
          break;
        }
      break;
    }
    case 'f':
    {
      if (LocaleCompare(option,"fish2x") == 0)
        {
          scaling_method=Fish2X;
          magnification=2;
          width=3;
          break;
        }
      break;
    }
    case 'h':
    {
      if (LocaleCompare(option,"hq2x") == 0)
        {
          scaling_method=Hq2X;
          magnification=2;
          width=3;
          break;
        }
      break;
    }
    case 's':
    {
      if (LocaleCompare(option,"scale2x") == 0)
        {
          scaling_method=Scale2X;
          magnification=2;
          width=3;
          break;
        }
      if (LocaleCompare(option,"scale3x") == 0)
        {
          scaling_method=Scale3X;
          magnification=3;
          width=3;
          break;
        }
      break;
    }
    case 'x':
    {
      if (LocaleCompare(option,"xbr2x") == 0)
        {
          scaling_method=Xbr2X;
          magnification=2;
          width=5;
        }
      break;
    }
    default:
      break;
  }
  /*
    Make a working copy of the source image and convert it to RGB colorspace.
  */
  source_image=CloneImage(image,image->columns,image->rows,MagickTrue,
    exception);
  offset.x=0;
  offset.y=0;
  rectangle.x=0;
  rectangle.y=0;
  rectangle.width=image->columns;
  rectangle.height=image->rows;
  (void) CopyImagePixels(source_image,image,&rectangle,&offset,exception);
  (void) SetImageColorspace(source_image,RGBColorspace,exception);
  magnify_image=CloneImage(source_image,magnification*source_image->columns,
    magnification*source_image->rows,MagickTrue,exception);
  if (magnify_image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Magnify the image.
  */
  status=MagickTrue;
  progress=0;
  image_view=AcquireVirtualCacheView(source_image,exception);
  magnify_view=AcquireAuthenticCacheView(magnify_image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(progress,status) \
    magick_number_threads(source_image,magnify_image,source_image->rows,1)
#endif
  for (y=0; y < (ssize_t) source_image->rows; y++)
  {
    Quantum
      r[128]; /* to hold result pixels */

    register Quantum
      *magick_restrict q;

    register ssize_t
      x;

    if (status == MagickFalse)
      continue;
    q=QueueCacheViewAuthenticPixels(magnify_view,0,magnification*y,
      magnify_image->columns,magnification,exception);
    if (q == (Quantum *) NULL)
      {
        status=MagickFalse;
        continue;
      }
    /*
      Magnify this row of pixels.
    */
    for (x=0; x < (ssize_t) source_image->columns; x++)
    {
      register const Quantum
        *magick_restrict p;

      size_t
        channels;

      register ssize_t
        i;

      ssize_t
        j;

      p=GetCacheViewVirtualPixels(image_view,x-width/2,y-width/2,width,width,
        exception);
      channels=GetPixelChannels(source_image);
      scaling_method(source_image,p,r,channels);
      /*
        Copy the result pixels into the final image.
      */
      for (j=0; j < (ssize_t) magnification; j++)
        for (i=0; i < (ssize_t) (channels*magnification); i++)
          q[j*channels*magnify_image->columns+i]=r[j*magnification*channels+i];
      q+=magnification*GetPixelChannels(magnify_image);
    }
    if (SyncCacheViewAuthenticPixels(magnify_view,exception) == MagickFalse)
      status=MagickFalse;
    if (image->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
        #pragma omp atomic
#endif
        progress++;
        proceed=SetImageProgress(image,MagnifyImageTag,progress,image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  magnify_view=DestroyCacheView(magnify_view);
  image_view=DestroyCacheView(image_view);
  source_image=DestroyImage(source_image);
  if (status == MagickFalse)
    magnify_image=DestroyImage(magnify_image);
  return(magnify_image);
}