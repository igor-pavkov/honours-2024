static double Welch(const double x,
  const ResizeFilter *magick_unused(resize_filter))
{
  magick_unreferenced(resize_filter);

  /*
    Welch parabolic windowing filter.
  */
  if (x < 1.0)
    return(1.0-x*x);
  return(0.0);
}