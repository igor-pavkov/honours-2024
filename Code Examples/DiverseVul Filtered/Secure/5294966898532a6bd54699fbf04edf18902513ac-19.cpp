MagickExport MagickBooleanType GetImageQuantizeError(Image *image,
  ExceptionInfo *exception)
{
  CacheView
    *image_view;

  double
    alpha,
    area,
    beta,
    distance,
    maximum_error,
    mean_error,
    mean_error_per_pixel;

  size_t
    index;

  ssize_t
    y;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  image->total_colors=GetNumberColors(image,(FILE *) NULL,exception);
  (void) memset(&image->error,0,sizeof(image->error));
  if (image->storage_class == DirectClass)
    return(MagickTrue);
  alpha=1.0;
  beta=1.0;
  area=3.0*image->columns*image->rows;
  maximum_error=0.0;
  mean_error_per_pixel=0.0;
  mean_error=0.0;
  image_view=AcquireVirtualCacheView(image,exception);
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    register const Quantum
      *magick_restrict p;

    register ssize_t
      x;

    p=GetCacheViewVirtualPixels(image_view,0,y,image->columns,1,exception);
    if (p == (const Quantum *) NULL)
      break;
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      index=GetPixelIndex(image,p);
      if (image->alpha_trait == BlendPixelTrait)
        {
          alpha=(double) (QuantumScale*GetPixelAlpha(image,p));
          beta=(double) (QuantumScale*image->colormap[index].alpha);
        }
      distance=fabs((double) (alpha*GetPixelRed(image,p)-beta*
        image->colormap[index].red));
      mean_error_per_pixel+=distance;
      mean_error+=distance*distance;
      if (distance > maximum_error)
        maximum_error=distance;
      distance=fabs((double) (alpha*GetPixelGreen(image,p)-beta*
        image->colormap[index].green));
      mean_error_per_pixel+=distance;
      mean_error+=distance*distance;
      if (distance > maximum_error)
        maximum_error=distance;
      distance=fabs((double) (alpha*GetPixelBlue(image,p)-beta*
        image->colormap[index].blue));
      mean_error_per_pixel+=distance;
      mean_error+=distance*distance;
      if (distance > maximum_error)
        maximum_error=distance;
      p+=GetPixelChannels(image);
    }
  }
  image_view=DestroyCacheView(image_view);
  image->error.mean_error_per_pixel=(double) mean_error_per_pixel/area;
  image->error.normalized_mean_error=(double) QuantumScale*QuantumScale*
    mean_error/area;
  image->error.normalized_maximum_error=(double) QuantumScale*maximum_error;
  return(MagickTrue);
}