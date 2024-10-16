MagickExport size_t GetNumberColors(const Image *image,FILE *file,
  ExceptionInfo *exception)
{
#define HistogramImageTag  "Histogram/Image"

  char
    color[MagickPathExtent],
    hex[MagickPathExtent],
    tuple[MagickPathExtent];

  PixelInfo
    *histogram;

  MagickBooleanType
    status;

  PixelInfo
    pixel;

  register PixelInfo
    *p;

  register ssize_t
    i;

  size_t
    number_colors;

  number_colors=0;
  if (file == (FILE *) NULL)
    {
      CubeInfo
        *cube_info;

      cube_info=ClassifyImageColors(image,exception);
      if (cube_info != (CubeInfo *) NULL)
        number_colors=cube_info->colors;
      cube_info=DestroyCubeInfo(image,cube_info);
      return(number_colors);
    }
  histogram=GetImageHistogram(image,&number_colors,exception);
  if (histogram == (PixelInfo *) NULL)
    return(number_colors);
  qsort((void *) histogram,(size_t) number_colors,sizeof(*histogram),
    HistogramCompare);
  GetPixelInfo(image,&pixel);
  p=histogram;
  status=MagickTrue;
  for (i=0; i < (ssize_t) number_colors; i++)
  {
    pixel=(*p);
    (void) CopyMagickString(tuple,"(",MagickPathExtent);
    ConcatenateColorComponent(&pixel,RedPixelChannel,X11Compliance,tuple);
    (void) ConcatenateMagickString(tuple,",",MagickPathExtent);
    ConcatenateColorComponent(&pixel,GreenPixelChannel,X11Compliance,tuple);
    (void) ConcatenateMagickString(tuple,",",MagickPathExtent);
    ConcatenateColorComponent(&pixel,BluePixelChannel,X11Compliance,tuple);
    if (pixel.colorspace == CMYKColorspace)
      {
        (void) ConcatenateMagickString(tuple,",",MagickPathExtent);
        ConcatenateColorComponent(&pixel,BlackPixelChannel,X11Compliance,
          tuple);
      }
    if (pixel.alpha_trait != UndefinedPixelTrait)
      {
        (void) ConcatenateMagickString(tuple,",",MagickPathExtent);
        ConcatenateColorComponent(&pixel,AlphaPixelChannel,X11Compliance,
          tuple);
      }
    (void) ConcatenateMagickString(tuple,")",MagickPathExtent);
    (void) QueryColorname(image,&pixel,SVGCompliance,color,exception);
    GetColorTuple(&pixel,MagickTrue,hex);
    (void) FormatLocaleFile(file,"%10.20g",(double) ((MagickOffsetType)
      p->count));
    (void) FormatLocaleFile(file,": %s %s %s\n",tuple,hex,color);
    if (image->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

        proceed=SetImageProgress(image,HistogramImageTag,(MagickOffsetType) i,
          number_colors);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
    p++;
  }
  (void) fflush(file);
  histogram=(PixelInfo *) RelinquishMagickMemory(histogram);
  if (status == MagickFalse)
    return(0);
  return(number_colors);
}