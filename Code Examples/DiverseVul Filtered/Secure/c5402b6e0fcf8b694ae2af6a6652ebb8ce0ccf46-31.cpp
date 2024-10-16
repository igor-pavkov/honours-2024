MagickPrivate double GetResizeFilterWeight(const ResizeFilter *resize_filter,
  const double x)
{
  double
    scale,
    weight,
    x_blur;

  /*
    Windowing function - scale the weighting filter by this amount.
  */
  assert(resize_filter != (ResizeFilter *) NULL);
  assert(resize_filter->signature == MagickCoreSignature);
  x_blur=fabs((double) x)/resize_filter->blur;  /* X offset with blur scaling */
  if ((resize_filter->window_support < MagickEpsilon) ||
      (resize_filter->window == Box))
    scale=1.0;  /* Point or Box Filter -- avoid division by zero */
  else
    {
      scale=resize_filter->scale;
      scale=resize_filter->window(x_blur*scale,resize_filter);
    }
  weight=scale*resize_filter->filter(x_blur,resize_filter);
  return(weight);
}