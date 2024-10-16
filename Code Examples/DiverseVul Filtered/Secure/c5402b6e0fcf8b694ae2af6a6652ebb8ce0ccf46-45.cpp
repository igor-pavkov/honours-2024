static double Cosine(const double x,
  const ResizeFilter *magick_unused(resize_filter))
{
  magick_unreferenced(resize_filter);

  /*
    Cosine window function:
      cos((pi/2)*x).
  */
  return((double)cos((double) (MagickPI2*x)));
}